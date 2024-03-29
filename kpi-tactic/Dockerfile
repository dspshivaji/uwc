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
LABEL description="KPI Tactic Application"

# Build KPI Tactic application
FROM ia_common:$EII_VERSION as kpitactic
WORKDIR /

# copy required dependencies
COPY KPIApp /KPIApp/. 
COPY --from=uwc_common /usr/yaml/include/yaml-cpp /usr/local/include/yaml-cpp
COPY --from=uwc_common /usr/yaml/lib/* /usr/local/lib/
COPY --from=uwc_common /usr/paho-cpp/lib/lib* /KPIApp/lib/
COPY --from=uwc_common /usr/lib/libpaho* /KPIApp/lib/
COPY --from=uwc_common /usr/ssl/lib/* /usr/local/lib/
COPY --from=uwc_common /usr/ssl/include/* /usr/local/include/
COPY --from=uwc_common /usr/paho-cpp/include/mqtt /usr/local/include/mqtt
COPY --from=uwc_common /usr/include/MQTT* /KPIApp/include/
COPY --from=uwc_common /EII/log4cpp/build/include/log4cpp /KPIApp/include/log4cpp
COPY --from=uwc_common /EII/log4cpp/build/lib/* /KPIApp/lib/
COPY --from=uwc_common /uwc_util/Release/libuwc-common.so /KPIApp/lib/
COPY --from=uwc_common /uwc_util/include/* /KPIApp/include/

# build sources
RUN cd /KPIApp/Release \
    &&	export LD_LIBRARY_PATH='/KPIApp/lib:/usr/local/lib' \
    &&	make clean all -s

# Build KPI App container service
FROM ia_common:$EII_VERSION as KPIApp-service

WORKDIR /
# creating base directories
ENV APP_WORK_DIR /opt/intel/app/
RUN mkdir -p /opt/intel/app \
	&& mkdir -p /opt/intel/app/logs \
	&& mkdir -p /opt/intel/config \
	&& mkdir -p /opt/intel/eii/uwc_data
WORKDIR ${APP_WORK_DIR}

# copy required dependencies
COPY --from=kpitactic /KPIApp/Release/KPIApp ${APP_WORK_DIR}
COPY --from=uwc_common /usr/yaml/lib/*.so.* ${APP_WORK_DIR}
COPY --from=uwc_common /usr/lib/libpaho* ${APP_WORK_DIR}
COPY --from=uwc_common /usr/ssl/lib/* /usr/local/lib/
COPY --from=uwc_common /usr/ssl/include/* /usr/local/include/
COPY --from=uwc_common /usr/paho-cpp/lib/lib* ${APP_WORK_DIR}
COPY --from=uwc_common /EII/log4cpp/build/lib/* ${APP_WORK_DIR}
COPY --from=kpitactic /KPIApp/Config/log4cpp.properties /opt/intel/config
COPY --from=uwc_common /uwc_util/Release/libuwc-common.so ${APP_WORK_DIR}

# set permissions to working dir for eiiuser
RUN chown -R ${EII_UID}:${EII_UID} /opt/intel/config \
   && chown -R ${EII_UID}:${EII_UID} ${APP_WORK_DIR} \
   && chmod -R 770 ${APP_WORK_DIR}

#set the environment variable
ENV LD_LIBRARY_PATH ${APP_WORK_DIR}:/usr/local/lib

# Run the container
CMD ./KPIApp
