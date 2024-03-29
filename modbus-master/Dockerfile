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
# SOFTWARE.

ARG EII_VERSION
FROM uwc_common:$EII_VERSION as uwc_common
LABEL description="Modbus TCP Master Application"

# Build Modbus Stack
FROM ia_common:$EII_VERSION as modbus-lib
WORKDIR /

# compile modbus stack
COPY modconn /modconn/.
WORKDIR modconn
RUN cd Release \
    && make clean all -s

# Build Modbus application
FROM ia_common:$EII_VERSION as modbus-app
WORKDIR /

# Copy built library from images
COPY --from=uwc_common /usr/yaml /bin/yaml-cpp/.

COPY Modbus-App /Modbus-App/. 
COPY --from=modbus-lib /modconn/Release/libModbusMasterStack.so /Modbus-App/lib/
COPY --from=modbus-lib /modconn/Inc/API.h /Modbus-App/include/
COPY --from=uwc_common /EII/log4cpp/build/include/log4cpp /Modbus-App/include/log4cpp
COPY --from=uwc_common /EII/log4cpp/build/lib/* /Modbus-App/lib/
COPY --from=uwc_common /uwc_util/Release/libuwc-common.so /Modbus-App/lib/
COPY --from=uwc_common /uwc_util/include/* /Modbus-App/include/
COPY --from=uwc_common /usr/paho-cpp/lib/lib* /Modbus-App/lib/
COPY --from=uwc_common /usr/lib/libpaho* /Modbus-App/lib/
COPY --from=uwc_common /usr/paho-cpp/include/mqtt /usr/local/include/mqtt

# compile modbus app sources
RUN cd /Modbus-App/Release \
    &&	export LD_LIBRARY_PATH='/Modbus-App/lib:/usr/local/lib\
    :/bin/yaml-cpp/lib' \
    &&  export INCLUDE_DIR='/bin' \
    &&	make clean all -s

# Build Modbus container service
FROM ia_common:$EII_VERSION as modbus-service
WORKDIR /

# creating base directories
ENV APP_WORK_DIR /opt/intel/app/
RUN mkdir -p /opt/intel/app \
	&& mkdir -p /opt/intel/app/logs \
	&& mkdir -p /opt/intel/config \
	&& mkdir -p /opt/intel/eii/uwc_data
WORKDIR ${APP_WORK_DIR}

# copy required libs and includes
COPY --from=modbus-lib /modconn/Release/libModbusMasterStack.so ${APP_WORK_DIR}
COPY --from=modbus-app /Modbus-App/Release/ModbusMaster ${APP_WORK_DIR}
COPY --from=uwc_common /usr/yaml/lib/*.so.* ${APP_WORK_DIR}
COPY --from=uwc_common /EII/log4cpp/build/lib/* ${APP_WORK_DIR}
COPY --from=modbus-app /Modbus-App/Config/log4cpp.properties /opt/intel/config
COPY --from=uwc_common /uwc_util/Release/libuwc-common.so ${APP_WORK_DIR}
COPY --from=uwc_common /usr/lib/libpaho* ${APP_WORK_DIR}
COPY --from=uwc_common /usr/paho-cpp/lib/lib* ${APP_WORK_DIR}

# set permissions to working dir for eiiuser
RUN chown -R ${EII_UID}:${EII_UID} /opt/intel/config \
   && chown -R ${EII_UID}:${EII_UID} ${APP_WORK_DIR} \
   && chmod -R 770 ${APP_WORK_DIR}

#set the environment variable
ENV LD_LIBRARY_PATH ${APP_WORK_DIR}:/usr/local/lib

# Run Modbus container
CMD ./ModbusMaster
