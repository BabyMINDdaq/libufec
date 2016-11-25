echo "Configuring..."
ufe-conf-gen -d | ufe-config -b 0 -d -s
ufe-conf-gen -d | ufe-config -b 3 -d -s

echo "Setting params..."
ufe-bpar-gen -d | ufe-set-param -b 0 -s -v
ufe-bpar-gen -d | ufe-set-param -b 3 -s -v

echo "Status is:"
ufe-read-status -b 0 -v
ufe-read-status -b 3 -v