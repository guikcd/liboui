#!/bin/sh
# TODO: use etag to see if file has changed
# 200 OK
# Connection: close
# Date: Tue, 24 Dec 2013 14:51:43 GMT
# Accept-Ranges: bytes
# ETag: "202724-3041af-4ee41b2080e80"
# Server: Apache
# Vary: Accept-Encoding
# Content-Length: 3162543
# Content-Type: text/plain; charset=utf-8
# Last-Modified: Tue, 24 Dec 2013 06:17:14 GMT
# Client-Date: Tue, 24 Dec 2013 14:51:43 GMT
# Client-Peer: 140.98.193.116:80
# Client-Response-Num: 1
rm -f oui_full.txt oui.txt

oui_url="http://standards.ieee.org/develop/regauth/oui/oui.txt"

#etag=$(curl --silent --head $oui_url | grep "^ETag: " | sed "s/^ETag: //")
# store etag for reuse
#echo $etag > oui_full.txt.etag

curl --compress "http://standards.ieee.org/develop/regauth/oui/oui.txt" > oui_full.txt
# just keep interesting lines
grep "(hex)" oui_full.txt > oui.txt
