#!/usr/bin/env python
# -*- coding: utf-8 -*-

# genscratchpad.py - A tool to generate encrypted scratchpad contents
#
# Requires:
#   - Python 2 v2.7 or newer (uses argparse, hextool.py)
#     or Python 3 v3.2 or newer
#   - PyCryptodome v3.0 or newer
#   - hextool.py in the same directory as this file

import sys
import os
import re
import zlib
import struct
import argparse
import textwrap

from Crypto.Hash import CMAC
from Crypto.Cipher import AES
from Crypto.Util import Counter
from Crypto.Random import get_random_bytes

# Import hextool.py, but do not write a .pyc file for it.
dont_write_bytecode = sys.dont_write_bytecode
sys.dont_write_bytecode = True
import hextool
sys.dont_write_bytecode = dont_write_bytecode
del dont_write_bytecode


# Python 2 and Python 3 support

try:
    # Python 3
    import configparser
    config_parser_tweaks = {"comment_prefixes": ("#", ";"),
                            "inline_comment_prefixes": (";", )}
except ImportError:
    # Python 2
    import ConfigParser as configparser
    config_parser_tweaks = {}

try:
    # Python 2
    xrange
except NameError:
    # Python 3
    xrange = range


# Constants

# Set to True to enable tests. Enable only one. See utils/aes.c for details.
# Results are written to the output file.
AES_TEST = False
CMAC_TEST = False

# Data block length, in bytes
BLOCK_LENGTH = 16

# Magic 16-byte string for locating a combi scratchpad in Flash
SCRATCHPAD_V1_TAG = b"SCR1\232\223\060\202\331\353\012\374\061\041\343\067"

# Maximum number of bytes in an input file
MAX_NUM_BYTES_PER_FILE = (8 * 1024 * 1024)  # Eight megabytes

# Maximum number of memory areas
MAX_NUM_MEMORY_AREAS = 8

# Maximum number of key pairs
MAX_NUM_KEYS = (1024 - MAX_NUM_MEMORY_AREAS * 16) // 32

# Number of bytes reserved for bootloader settings
BOOTLOADER_SETTINGS_NUM_BYTES = (0x2000 - 0x1c00)

# Input data for testing AES and CMAC
test_data = bytearray([
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11
    ])

# Initial Counter Block for AES CTR test
test_icb = bytearray([
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
    ])


# Classes

class InFile(object):
    '''An input file'''

    def __init__(self, file_spec = None, data = None,
                 version = (0, 0, 0, 0)):
        if file_spec == None and data == None:
            raise ValueError("no data given")
        elif file_spec != None and data != None:
            raise ValueError("file_spec and data are mutually exclusive")

        # Set default values.
        self.area_id = 0x00000000
        self.version = version
        self.compressed = False
        self.encrypted = False

        if data:
            # Data given, make a copy of it.
            self.data = bytearray(data)
        else:
            # File specification given, parse it.
            version, self.area_id, filename = self.parse_file_spec(file_spec)
            self.version = parse_version(version, self.version[3])

            # Read data from file.
            memory = hextool.Memory()
            hextool.load_intel_hex(memory, filename = filename)

            if memory.num_ranges == 0:
                # No data found in file.
                raise ValueError("file contains no data: '%s'" % filename)
            elif (memory.max_address -
                  memory.min_address > MAX_NUM_BYTES_PER_FILE):
                raise ValueError("file too big: '%s'" % filename)

            # Convert Memory object to a flat bytearray.
            self.data = memory[memory.min_address:memory.max_address]

        # Create a file header for uncompressed, unencrypted data.
        self.header = make_file_header(self.area_id, len(self.data),
                                       *self.version)

    def compress(self):
        if self.compressed:
            raise ValueError("data already compressed")

        # OPTIMIZE: Avoid extra conversions between bytearrays and bytes.
        # Compress data. zlib.compress() does not accept
        # bytearrays, so convert bytearray to bytes.
        compressed_data = bytearray(zlib.compress(bytes(self.data), 9))

        # Determine compressed data length without
        # zlib header and Adler-32 checksum.
        comp_data_len = len(compressed_data) - 2 - 4

        # Pad size to a multiple of block length.
        num_blocks = (comp_data_len + BLOCK_LENGTH - 1) // BLOCK_LENGTH

        # Mark data as compressed.
        self.compressed = True

        # Create a bytearray object for compressed data.
        self.data = bytearray(num_blocks * BLOCK_LENGTH)

        # Copy compressed data to bytearray, but leave
        # out zlib header and Adler-32 checksum.
        self.data[:comp_data_len] = compressed_data[2:-4]

        # Update the file header now that data length has changed.
        self.header = make_file_header(self.area_id, len(self.data),
                                       *self.version)

    def encrypt(self, cipher):
        if self.encrypted:
            raise ValueError("data already encrypted")

        # OPTIMIZE: Avoid extra conversions between bytearrays and bytes.
        # Cipher does not accept bytearrays, so convert to bytes.
        self.data = bytearray(cipher.encrypt(bytes(self.data)))

        # Mark data as encrypted.
        self.encrypted = True

    def parse_file_spec(self, file_spec):
        '''
        Parse input file specification.

        file_spec   version:area_id:filename
        '''

        try:
            # Read version, memory area ID and file name.
            version, area_id, filename = file_spec.split(":", 2)

            area_id = int(area_id, 0)
            if area_id < 0 or area_id > 4294967295:
                raise ValueError
        except ValueError:
            raise ValueError("invalid input file specification: "
                             "'%s'" % file_spec)

        return version, area_id, filename


# Functions

def crc16_ccitt(data, initial = 0xffff):
    '''Simple and slow version of CRC16-CCITT'''

    # OPTIMIZE: Avoid extra conversions between bytearrays and bytes.
    crc = initial
    data = bytearray(data)

    for b in data:
        crc = (crc >> 8) | ((crc & 0xff) << 8)
        crc ^= b
        crc ^= (crc & 0xf0) >> 4
        crc ^= (crc & 0x0f) << 12
        crc ^= (crc & 0xff) << 5

    return crc


def create_cipher(icb, cipher_key):
    '''
    Create an AES-128 Counter (CTR) mode cipher
    with 16-byte initial counter block.
    '''

    # AES.new() only accepts bytes, not bytearray.
    cipher_key = bytes(cipher_key)

    # Create a fast counter for AES cipher.
    icb0, icb1, icb2, icb3 = struct.unpack("<4L", icb)
    ctr = Counter.new(128, little_endian = True,
                      allow_wraparound = True,
                      initial_value = (icb3 << 96) | (icb2 << 64) |
                                      (icb1 << 32) | icb0)

    # Create an AES Counter (CTR) mode cipher.
    return AES.new(cipher_key, AES.MODE_CTR, counter = ctr)


def calculate_cmac(data_list, auth_key):
    '''Calculate CMAC / OMAC1 tag for data.'''

    # CMAC.new() only accepts bytes, not bytearray.
    auth_key = bytes(auth_key)

    # Create a CMAC / OMAC1 object.
    cobj = CMAC.new(auth_key, ciphermod = AES)

    for data in data_list:
        # OPTIMIZE: Avoid extra conversions between bytearrays and bytes.
        # CMAC object does not accept bytearrays, so convert to bytes.
         cobj.update(bytes(data))

    # Calculate digest and return it as bytearray.
    return bytearray(cobj.digest())


def parse_version(version, default_devel):
    '''Parse a version number of form MINOR.MAJOR.MAINTENANCE[.DEVELOPMENT].'''

    try:
        # Split to components.
        ver_components = version.strip().split(".")

        if len(ver_components) < 3 or len(ver_components) > 4:
            # Wrong number of components.
            raise ValueError
        elif len(ver_components) == 3:
            # No development number, use default.
            ver_components.append(default_devel)

        # Convert to integers.
        ver_components = [int(n) for n in ver_components]
    except ValueError:
        raise ValueError("invalid firmware version: '%s'" % version)

    return tuple(ver_components)


def create_argument_parser(pgmname):
    '''Create a parser for parsing the command line.'''

    # Determine help text width.
    try:
        help_width = int(os.environ['COLUMNS'])
    except (KeyError, ValueError):
        help_width = 80
    help_width -= 2

    # Load "genscratchpad.ini" from the program directory by default.
    def_configfile = os.path.join(os.path.dirname(sys.argv[0]),
                                  "genscratchpad.ini")

    parser = argparse.ArgumentParser(
        prog = pgmname,
        formatter_class = argparse.RawDescriptionHelpFormatter,
        description = textwrap.fill(
            "A tool to generate compressed and "
            "encrypted scratchpad contents", help_width),
        epilog =
            "values:\n"
            "  infilespec    version:area_id:filename\n"
            "  area_id       0x00000000 .. 0xffffffff\n"
            "  version       major.minor.maintenance[.developer], "
            "each 0 .. 255,\n"
            "                developer defaults to "
            "OTAP sequence number if not given")
    parser.add_argument("--configfile", "-c", default = def_configfile,
        metavar = "FILE",
        help = "a configuration file with keys and memory information "
               "(default: \"%(default)s\")")
    parser.add_argument("--bootloader", "-b",
        metavar = "FILE",
        help = "a bootloader binary file")
    parser.add_argument("--genprog", "-p",
        metavar = "FILE",
        help = "produce a programming output file in addition to "
               "scratchpad, in Intel HEX format")
    parser.add_argument("--keyname", "-k",
        metavar = "NAME", default = "default",
        help = "name of key to use for encryption and authentication "
               "(default: \"%(default)s\")")
    parser.add_argument("--otapseq", "-o", type = int, default = 1,
        metavar = "NUM",
        help = "OTAP sequence number of output file "
               "(0 to 255, default: %(default)s)")
    parser.add_argument("--type", "-t", metavar = "TYPE", type = str,
        default = "combi",
        help = "type of scratchpad to generate: combi "
               "(default: \"%(default)s\")")
    parser.add_argument("--set", "-s", action = "append", type = str,
        metavar = "SYMBOL=VALUE", default = [], dest = "symbols",
        help = "introduce a symbolic value")
    parser.add_argument("outfile", metavar = "OUTFILE",
        help = "compressed, encrypted output scratchpad file")
    parser.add_argument("infilespec", nargs = "*", metavar = "INFILESPEC",
        help = "input file(s) in Intel HEX format to "
               "be added in scratchpad, see below")

    return parser


def validate_symbol_name(name):
    '''Validate a symbol name:

    letters, digits or underscore, except no digits in the first position
    '''

    first = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz"
    rest = "0123456789" + first

    if len(name) == 0:
        return False
    elif name[0] not in first:
        return False

    for c in name[1:]:
        if c not in rest:
            return False

    return True


def parse_arguments(parser):
    '''Parse command line arguments.'''

    args = parser.parse_args()

    if args.genprog and not args.bootloader:
        raise ValueError("cannot generate programming output file "
                         "without bootloader")

    sptype = args.type.lower()

    if sptype != "combi":
        raise ValueError("invalid scratchpad type: '%s'" % args.type)

    # Convert a list of symbol definitions to a dictionary.
    symbols = {}
    for sdef in args.symbols:
        sname, svalue = sdef.split("=", 1)

        if not validate_symbol_name(sname):
            raise ValueError("invalid symbol name: '%s'" % sname)

        symbols[sname] = svalue

    args.symbols = symbols

    return args


def make_file_header(areaid, length,
                     ver_major, ver_minor, ver_maint, ver_devel):
    '''
    From bootloader.h:

    struct
    {
        /** Memory area ID of this item */
        uint32_t id;
        /** Number of compressed bytes following this header */
        uint32_t length;
        /** Firmware major version number component */
        uint8_t major;
        /** Firmware minor version number component */
        uint8_t minor;
        /** Firmware maintenance version number component */
        uint8_t maint;
        /** Firmware development version number component */
        uint8_t devel;
        /** Padding, reserved for future use, must be 0 */
        uint32_t pad;
    };
    '''

    if areaid < 0 or areaid > 4294967295:
        raise ValueError("memory area ID not 0 .. 4294967295")

    pad = 0x00000000

    return struct.pack("<2L4BL", areaid, length,
                       ver_major, ver_minor, ver_maint, ver_devel, pad)


def make_scratchpad_header(otap_seq, scratchpad_data):
    '''
    From bootloader.h:

    struct
    {
        /** Number of bytes, not including header */
        uint32_t length;
        /** CRC16-CCITT, not including any header bytes */
        uint16_t crc;
        /** Sequence number of data in scratchpad: 0 .. 255 */
        bl_scratchpad_seq_t seq;    /* uint8_t */
        /** Padding, reserved for future use, must be 0 */
        uint8_t pad;
        /** Scratchpad type information for bootloader: bl_header_type_e */
        uint32_t type;
        /** Status code from bootloader: bl_header_status_e */
        uint32_t status;
    };

    typedef enum
    {
        ...
        /** Scratchpad contains data that the bootloader can process */
        BL_SCRATCHPAD_TYPE_PROCESS                  = 0x00000000
    } bl_scratchpad_type_e;

    typedef enum
    {
        /** Bootloader has not yet processed the scratchpad contents */
        BL_SCRATCHPAD_STATUS_NEW                    = 0xFFFFFFFF,
        ...
    } bl_scratchpad_status_e;
    '''

    if otap_seq < 0 or otap_seq > 255:
        raise ValueError("sequence number not 0 .. 255")

    # Calculate length of scratchpad contents.
    length = sum(map(len, scratchpad_data))

    if length % 16 != 0:
        raise ValueError("data length not multiple of 16")

    # Calculate CRC of scratchpad contents.
    crc = 0xffff
    for data in scratchpad_data:
        crc = crc16_ccitt(data, crc)

    pad = 0x00

    bltype = 0x00000000     # BL_HEADER_TYPE_FIRMWARE
    blstatus = 0xffffffff   # BL_HEADER_STATUS_NEW

    return struct.pack("<LH2B2L", length, crc, otap_seq, pad, bltype, blstatus)


def gen_prog_image(config, bootloader, scratchpad_header = None, scratchpad_data = None):
    '''Create a Flash memory image, ready for programming to a device.'''

    if scratchpad_header is None and scratchpad_data is not None \
            or scratchpad_header is not None and scratchpad_data is None:
        raise ValueError("scratchpad_header and scratchpad_data must be both set or both unset")

    bl_start = config["bootloader.address"]
    bl_max_num_bytes = config["bootloader.length"]
    bl_end = bl_start + bl_max_num_bytes
    set_start = config["bootloader.address"] + config["bootloader.settings"]
    set_key_start = set_start + MAX_NUM_MEMORY_AREAS * 16
    set_end = set_start + BOOTLOADER_SETTINGS_NUM_BYTES
    sp_start = config["scratchpad.address"]
    sp_max_num_bytes = config["scratchpad.length"]
    sp_end = config["scratchpad.top"]

    # Create a Memory object to store the Flash memory image.
    memory = hextool.Memory()

    if scratchpad_data is not None:
        # Calculate total scratchpad length in bytes.
        sp_num_bytes = sum(map(len, scratchpad_data))

        if sp_num_bytes > sp_max_num_bytes:
            raise ValueError("scratchpad too big by %d bytes" % (sp_num_bytes -
                                                                 sp_max_num_bytes))

    if bootloader.min_address < bl_start : # DEBUG: or bootloader.max_address > bl_end:
        raise ValueError("bootloader overflows outside its area: "
                         "0x%04x .. 0x%04x, should be 0x%04x .. 0x%04x" %
                         (bootloader.min_address, bootloader.max_address,
                          bl_start, bl_end))

    # Add bootloader to the Flash memory image.
    memory.cursor = bl_start
    memory += bootloader

    # Erase keys and memory area information Flash memory image.
    del memory[set_start:set_end]

    # Set Memory cursor to point at the bootloader area settings.
    memory.cursor = set_start

    # Add memory area information in Flash memory image.
    for area_name in config["areas"]:
        area = config["areas"][area_name]
        data = struct.pack("<4L", area["address"], area["length"],
                           area["id"], area["flags"])
        memory += data

    # Sanity check.
    if memory.cursor > set_key_start:
        raise ValueError("error generating programming image: bootloader areas")

    # Set Memory cursor to point at the bootloader key settings.
    memory.cursor = set_key_start

    # Add keys in Flash memory image.
    for key_name in config["keys"]:
        keypair = config["keys"][key_name]
        memory += keypair["auth"]
        memory += keypair["encrypt"]

    # Sanity check.
    if memory.cursor > set_end:
        raise ValueError("error generating programming image: bootloader keys")

    if scratchpad_data is not None:
        # Set Memory cursor to point at the first byte
        # of scratchpad in the Flash memory image.
        memory.cursor = (sp_end - len(SCRATCHPAD_V1_TAG) -
                     len(scratchpad_header) - sp_num_bytes)

        # Add scratchpad to the Flash memory image.
        for data in scratchpad_data:
            memory += data

        # Add a scratchpad header at the end, followed by a 16-byte tag
        # for finding the scratchpad in Flash, to the Flash memory image.
        memory += scratchpad_header
        memory += SCRATCHPAD_V1_TAG

        # Sanity check.
        if memory.cursor != sp_end:
            raise ValueError("error generating programming image: scratchpad")

    return memory


def lookup_symbol(value, symbols):
    '''Get a symbolic value or use value as is'''
    return symbols.get(value, value)


def lookup_int(value, symbols):
    '''Get a symbolic or numeric value as integer'''

    value = lookup_symbol(value, symbols)

    try:
        value = int(value, 0)
    except ValueError:
        raise ValueError("invalid value: '%s'" % value)

    return value


def get_string(value):
    '''Parse a string value and remove enclosing quotes'''
    try:
        if len(value) == 0:
            return ""
        elif len(value) < 2:
            raise ValueError
        elif value[0] != '"' or value[-1] != '"':
            raise ValueError
        return value[1:-1]
    except ValueError:
        raise ValueError("invalid value: '%s'" % value)


def get_key(value):
    '''Parse an encryption or authentication key value

    Bytes are separated by commas or spaces.'''
    value = value.replace(",", " ")
    return bytearray.fromhex(value)


def read_config(configfile, symbols, chosenkeyname = None):
    '''Read and parse a configuration file.'''

    # Read configuration file.
    with open(configfile, "rU") as f:   # Universal newlines
        config_parser = configparser.RawConfigParser(
            **config_parser_tweaks) # Most basic parser only
        config_parser.readfp(f)

    class HasToBeString(object):
        '''Marker for missing string configuration value'''
        pass

    class HasToBeInt(object):
        '''Marker for missing integer configuration value'''
        pass

    class HasToBeKey(object):
        '''Marker for missing encryption or auth key configuration value'''
        pass

    # Configurable options: {"section.item" = value, ...}
    config = {
        "flash.length":         HasToBeInt,
        "flash.eraseblock":     HasToBeInt,
        "bootloader.area":      HasToBeString,
        "bootloader.settings":  HasToBeInt,
        "bootloader.address":   HasToBeInt,
        "bootloader.length":    HasToBeInt,
        "scratchpad.area":      HasToBeString,
        "scratchpad.address":   HasToBeInt,
        "scratchpad.top":       HasToBeInt,
        "scratchpad.length":    HasToBeInt
    }

    # Area names
    areas = []

    # Key names
    keys = []

    # A list of sections that have been seen
    sections = []

    for section in config_parser.sections():
        if section in sections:
            # Section already seen.
            raise ValueError("section '%s' already defined" % section)

        # This section has now been seen.
        sections.append(section)

        # Check section name against known patterns: "area:" and "key:".
        sre_area = re.match("area:([A-Za-z0-9_]+)", section)
        sre_key = re.match("key:([A-Za-z0-9_]+)", section)

        # Identify an "area:" section.
        if sre_area != None:
            area_name = sre_area.group(1)
            config["%s.id" % section] = HasToBeInt
            config["%s.address" % section] = HasToBeInt
            config["%s.length" % section] = HasToBeInt
            config["%s.flags" % section] = HasToBeInt
            areas.append(area_name)

        # Identify a "key:" section.
        if sre_key != None:
            key_name = sre_key.group(1)
            config["%s.auth" % section] = HasToBeKey
            config["%s.encrypt" % section] = HasToBeKey
            keys.append(key_name)

        # Parse sections.
        for item, value in config_parser.items(section):
            sec_item = "%s.%s" % (section, item)
            if sec_item not in config:
                raise ValueError("unknown section '%s' item '%s'" %
                                 (section, item))
            else:
                if sec_item not in config:
                    raise ValueError("unknown item '%s' in section '%s'" %
                                     (item, section))
                elif config[sec_item] == HasToBeInt:
                    config[sec_item] = lookup_int(value, symbols)
                elif config[sec_item] == HasToBeString:
                    config[sec_item] = get_string(value)
                elif config[sec_item] == HasToBeKey:
                    config[sec_item] = get_key(value)
                else:
                    raise ValueError("duplicate item '%s' in section '%s'" %
                                     (item, section))
                continue

    if "flash" not in sections:
        raise ValueError("no flash section in configuration file")

    if "bootloader" not in sections:
        raise ValueError("no bootloader section in configuration file")

    if "scratchpad" not in sections:
        raise ValueError("no scratchpad section in configuration file")

    if len(areas) == 0:
        raise ValueError("no memory areas in configuration file")
    elif len(areas) > MAX_NUM_MEMORY_AREAS:
        raise ValueError("too many memory areas in configuration file")

    if len(keys) == 0:
        raise ValueError("no keys in configuration file")
    if len(keys) > MAX_NUM_KEYS:
        raise ValueError("too many keys in configuration file")

    # Set default flag values for areas, where not set.
    for area_name in areas:
        section = "area:%s" % area_name
        if config["%s.flags" % section] == HasToBeInt:
            config["%s.flags" % section] = 0x00000000

    # Check that the bootloader has an area name, or
    # alternatively an address and a length.
    areaname = config["bootloader.area"]
    if areaname != HasToBeString:
        # Check that the address and length options are not given, too.
        if (config["bootloader.address"] != HasToBeInt or
            config["bootloader.length"] != HasToBeInt):
            raise ValueError("bootloader area name is mutually exclusive to "
                             "address and length")

        if areaname not in areas:
            raise ValueError("unknown area '%s'" % areaname)

        # Take bootloader address and length from an area definition.
        config["bootloader.address"] = config["area:%s.address" % areaname]
        config["bootloader.length"] = config["area:%s.length" % areaname]
    else:
        # No area name given, use address and length instead.
        config["bootloader.area"] = ""

        if config["bootloader.address"] == HasToBeInt:
            raise ValueError("no bootloader address given")

        config["bootloader.address"] = config["bootloader.address"]

        if config["bootloader.length"] == HasToBeInt:
            raise ValueError("no bootloader length given")

        config["bootloader.length"] = config["bootloader.length"]

    # Check that the scratchpad has an area name, or alternatively
    # an address and a length, or a top address and a length.
    areaname = config["scratchpad.area"]
    if areaname != HasToBeString:
        # Check that the address, top address and
        # length options are not given, too.
        if (config["scratchpad.address"] != HasToBeInt or
            config["scratchpad.top"] != HasToBeInt or
            config["scratchpad.length"] != HasToBeInt):
            raise ValueError("scratchpad area name is mutually exclusive to "
                             "address, top address and length")

        if areaname not in areas:
            raise ValueError("unknown area '%s'" % areaname)

        # Take scratchpad address and length from an area definition.
        config["scratchpad.address"] = config["area:%s.address" % areaname]
        config["scratchpad.length"] = config["area:%s.length" % areaname]
        config["scratchpad.top"] = (config["scratchpad.address"] +
                                    config["scratchpad.length"])
    else:
        # No area name given, use address (or top) and length instead.
        config["scratchpad.area"] = ""

        if (config["scratchpad.address"] != HasToBeInt and
            config["scratchpad.top"] != HasToBeInt):
            raise ValueError("scratchpad address and top "
                             "address are mutually exclusive")

        if config["scratchpad.length"] == HasToBeInt:
            raise ValueError("no scratchpad length given")

        config["scratchpad.length"] = config["scratchpad.length"]

        # Calculate start or top address for scratchpad.
        if config["scratchpad.address"] != HasToBeInt:
            config["scratchpad.top"] = (config["scratchpad.address"] +
                                        config["scratchpad.length"])
        elif config["scratchpad.top"] != HasToBeInt:
            config["scratchpad.address"] = (config["scratchpad.top"] -
                                            config["scratchpad.length"])
        else:
            raise ValueError("no scratchpad address given")

    # Find if the scratchpad is located in internal or external flash.
    # Flags = 0x12 means Scratchpad area is located in external flash.
    if config["scratchpad.area"] == "scratchpad" and \
        config["area:scratchpad.flags"] == 0x12:
        config["scratchpad.external"] = True
    else:
        config["scratchpad.external"] = False

    # Check that there are no unset items that are mandatory.
    for sec_item in config:
        if config[sec_item] in (HasToBeString, HasToBeInt, HasToBeKey):
            section, item = sec_item.split(".", 1)
            raise ValueError("missing item '%s' in section '%s'" %
                             (item, section))

    # Check that the bootloader settings can fit in the given area.
    param = config["bootloader.settings"]
    if (param == HasToBeInt or
        param + BOOTLOADER_SETTINGS_NUM_BYTES > config["bootloader.length"]):
        raise ValueError("invalid bootloader settings offset")

    # Convert areas to a virtual configuration section.
    config_areas = {}
    for area in areas:
        config_area = {}
        config_area["id"] = config["area:%s.id" % area]
        config_area["address"] = config["area:%s.address" % area]
        config_area["length"] = config["area:%s.length" % area]
        config_area["flags"] = config["area:%s.flags" % area]
        config_areas[area] = config_area
    config["areas"] = config_areas

    # Convert keys to a virtual configuration section.
    config_keys = {}
    for key in keys:
        config_key = {}
        config_key["auth"] = config["key:%s.auth" % key]
        config_key["encrypt"] = config["key:%s.encrypt" % key]
        config_keys[key] = config_key
    config["keys"] = config_keys

    # Convert chosen keys to a virtual configuration section, if a key given.
    if chosenkeyname != None:
        if chosenkeyname not in config["keys"]:
            raise ValueError("key not found: '%s'" % chosenkeyname)
        config["key.auth"] = config["keys"][chosenkeyname]["auth"]
        config["key.encrypt"] = config["keys"][chosenkeyname]["encrypt"]

    return config


def main():
    '''Main program'''

    # Determine program name, for error messages.
    pgmname = os.path.split(sys.argv[0])[-1]

    # Create a parser for parsing the command line and printing error messages.
    parser = create_argument_parser(pgmname)

    try:
        # Parse command line arguments.
        args = parse_arguments(parser)

        # Load configuration file.
        config = read_config(args.configfile, args.symbols, args.keyname)
    except (ValueError, IOError, OSError, configparser.Error) as exc:
        sys.stdout.write("%s: %s\n" % (pgmname, exc))
        return 1

    if False:
        # DEBUG: Show config.
        keys = list(config.keys())
        keys.sort()
        for key in keys:
            print("%40s: %s" % (key, repr(config[key])))

    # Create a Memory object for storing the bootloader.
    bootloader = hextool.Memory()

    if args.bootloader:
        # Read bootloader.
        hextool.load_intel_hex(bootloader, filename = args.bootloader)

    in_files = []           # List of InFile objects
                            # (header, area_id, version, data)
    scratchpad_data = []    # List of scratchpad data blocks
    ver_major, ver_minor, ver_maint, ver_devel = (0, 0, 0, 0)

    if AES_TEST:
        # Run AES test. See aes_test1() in utils/aes.c for details.
        in_files.append(InFile(data = test_data))
        scratchpad_data.append(test_icb)
    elif CMAC_TEST:
        # Run CMAC / OMAC1 test. See aes_omac1_test1()
        # in utils/aes.c for details.
        scratchpad_data.append(test_data)
    else:
        # Read input files.
        try:
            for file_spec in args.infilespec:
                # Create an InFile object.
                in_file = InFile(file_spec = file_spec,
                                 version = (0, 0, 0, args.otapseq))

                # Compress data in-place.
                in_file.compress()

                in_files.append(in_file)
        except (ValueError, IOError, OSError) as exc:
            sys.stdout.write("%s: %s\n" % (pgmname, exc))
            return 1

        if len(in_files) == 0 and args.genprog != None:
            # Combine bootloader, memory area specification and keys (no scratchpad).
            # Generate a programming image and save it as Intel HEX.
            memory = gen_prog_image(config, bootloader)
            hextool.save_intel_hex(memory, filename=args.genprog)
            return 0

        # Create secure header, which is also the initial counter block (ICB).
        secure_header = get_random_bytes(16)
        scratchpad_data.append(secure_header)


    if not CMAC_TEST:
        try:
            # Create an AES Counter (CTR) mode cipher using secure
            # header as the 16-byte initial counter block (ICB).
            cipher = create_cipher(secure_header, config["key.encrypt"])

            # Encrypt each input file.
            for in_file in in_files:
                # Encrypt data in-place.
                in_file.encrypt(cipher)

                # Add file header to scratchpad data.
                scratchpad_data.append(in_file.header)

                # Add compressed, encrypted file data to scratchpad data.
                scratchpad_data.append(in_file.data)
        except ValueError as exc:
            sys.stdout.write("%s: %s\n" % (pgmname, exc))
            return 1

    # Calculate and add CMAC / OMAC1 tag.
    try:
        cmac = calculate_cmac(scratchpad_data, config["key.auth"])
        scratchpad_data.insert(0, cmac)
    except ValueError as exc:
        sys.stdout.write("%s: %s\n" % (pgmname, exc))
        return 1

    if not AES_TEST and not CMAC_TEST:
        # Create a scratchpad header.
        try:
            scratchpad_header = make_scratchpad_header(args.otapseq,
                                                       scratchpad_data)
        except ValueError as exc:
            sys.stdout.write("%s: %s\n" % (pgmname, exc))
            return 1
    else:
        scratchpad_header = bytes()

    # Write output file and optionally the programming image file.
    try:
        with open(args.outfile, "wb") as f:
            # A combi scratchpad file starts with a 16-byte tag.
            f.write(SCRATCHPAD_V1_TAG)

            # Combi scratchpad files have the scratchpad header in front of
            # the scratchpad contents. The firmware rearranges the data in
            # Flash memory while storing the scratchpad.
            f.write(scratchpad_header)

            # Write scratchpad contents.
            for data in scratchpad_data:
                f.write(data)

        # Combine bootloader, memory area specification, keys and scratchpad.
        if args.genprog != None and config["scratchpad.external"] == False:
            # Generate a programming image and save it as Intel HEX.
            memory = gen_prog_image(config, bootloader,
                                    scratchpad_header, scratchpad_data)
            hextool.save_intel_hex(memory, filename = args.genprog)
    except (ValueError, IOError, OSError) as exc:
        sys.stdout.write("%s: %s\n" % (pgmname, exc))
        return 1

    return 0


# Run main.
if __name__ == "__main__":
    sys.exit(main())
