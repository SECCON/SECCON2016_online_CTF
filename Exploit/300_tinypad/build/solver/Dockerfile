### docker run -t your_name/tinypad:latest .
FROM        ubuntu:16.04
MAINTAINER  tinypad

# Update the system
RUN         apt-get update
RUN         apt-get -y upgrade
RUN         apt-get -y install socat

# Create a user, tinypad
RUN         useradd -m tinypad \
            && echo "tinypad:tinypad"| chpasswd

# Deploy the problem
ADD         ./tinypad.tar /home/
RUN         chown -R root:tinypad /home/tinypad                                 \
            && chmod 750 /home/tinypad                                          \
            && chmod 770 /home/tinypad/tinypad_ffe00c8f7dfd94a8c024f54cf155f8b2 \
            && chown root:tinypad /home/tinypad/flag.txt                        \
            && chmod 440 /home/tinypad/flag.txt

# Get a `tini'
ADD         https://github.com/krallin/tini/releases/download/v0.13.0/tini_0.13.0.deb /tmp
RUN         dpkg -i /tmp/tini_0.13.0.deb
ENTRYPOINT  ["/usr/bin/tini", "--"]

# Enter into the system as tinypad
USER        tinypad
WORKDIR     /home/tinypad
ENV         HISTFILE /dev/null

CMD         socat TCP-LISTEN:57463,reuseaddr,fork EXEC:/home/tinypad/tinypad_ffe00c8f7dfd94a8c024f54cf155f8b2
