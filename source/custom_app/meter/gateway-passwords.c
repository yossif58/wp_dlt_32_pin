Hi Charles,
It seems that a lot of things slipped from my understanding.
0.I started this project following the readme file from https://github.com/wirepas/gateway.
1.      repo init -u https://github.com/wirepas/manifest.git \
              -m gateway/stable.xml \
              --no-clone-bundle
2.    repo sync
3. sudo apt install libsystemd-dev
4. sudo apt install libsystemd-dev python3 python3-dev python3-gi
5. wget https://bootstrap.pypa.io/get-pip.py \
       && sudo python3 get-pip.py && rm get-pip.py \
       && sudo pip3 install --upgrade pip


Hi Yossi,


The first few lines of that page mention what you are about to setup and what the dependencies are:

"This repository contains Wirepas' reference gateway implementation, which relies on a set of services to exchange data from/to a Wirepas Mesh network from/to a MQTT broker or host device."


That is what this gateway does, it takes wirepas network data and it helps you send it to an MQTT broker or host device for further use (analytics or whatever you want to do with it)

So it sort of assumes that you have a MQTT Broker setup as you want to do something with your network data most of the time.

Like I said in my previous email, you have an MQTT server, as I did setup the whole backend weeks ago for you already.

See the customer_report.pdf file on Sharefile that has the server info and everything else you need in it.

What you want is the server address: powercomwnt.extwirepas.com


Gateway MQTT credentials 
User name: mosquittouser
Password: wf8ICxoJCdDPWh5v9JIvEfGEsgZ
and the port: 8883


Br,
Charles
 





