# Copyright (c) 2021 Intel Corporation.

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE

ARG EII_VERSION
FROM ia_eiibase:$EII_VERSION as eiibase
LABEL description="MQTT container"
ARG EII_UID
ARG EII_USER_NAME

FROM eiibase
WORKDIR /

RUN apt-get update && apt-get install -y mosquitto mosquitto-clients
RUN mkdir -p /mosquitto/config /mosquitto/data /mosquitto/log
COPY mosquitto_prod.conf /etc/mosquitto/mosquitto_prod.conf
COPY mosquitto_dev.conf /etc/mosquitto/mosquitto_dev.conf

COPY mqtt_broker_start.sh /
RUN chmod +x mqtt_broker_start.sh

CMD ["./mqtt_broker_start.sh"]
