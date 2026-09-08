SUMMARY = "A small collection of utilities to aid in firmware development for the Glowforge laser cutter."

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENSE;md5=26acf3c796ca3fbbf3a264522a4a9065"

BBCLASSEXTEND = "native nativesdk"

# gfutilities 0.9.1 install_requires: requests, urllib3, websocket-client.
# (0.9.1 replaced the old lomond websocket lib with websocket-client. gfutilities
# eagerly imports its WSS service at package load, so anything importing it  - 
# e.g. python3-gfhardware - fails with ModuleNotFoundError: 'websocket' unless
# websocket-client is installed.)
RDEPENDS:${PN} += "\
    ${PYTHON_PN}-requests \
    ${PYTHON_PN}-urllib3 \
    ${PYTHON_PN}-websocket-client \
    "
# The standard-library packages the modules import.
RDEPENDS:${PN} += " \
    ${PYTHON_PN}-core \
    ${PYTHON_PN}-datetime \
    ${PYTHON_PN}-io \
    ${PYTHON_PN}-json \
    ${PYTHON_PN}-logging \
    ${PYTHON_PN}-threading \
"
