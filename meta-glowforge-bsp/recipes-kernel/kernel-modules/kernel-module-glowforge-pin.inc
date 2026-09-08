# kernel-module-glowforge pin. Bump deliberately after pushing module
# changes; keep only SRCREV and PV here - the ForgeFIRM image manifest
# leaves *-pin.inc out of the layer content hash because the component
# entry already identifies the pinned source.
SRCREV = "d8773e20b04469ca6efd5f36ad9c2101e75d60b4"
PV = "0.0.5"
