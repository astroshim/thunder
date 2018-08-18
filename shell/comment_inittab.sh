#!/bin/bash

sed '/dsm:2345:respawn:\/wizbong\/bin\/DSMgr/ s/^/#/g' /etc/inittab.ins > /etc/inittab2.ins
mv /etc/inittab2.ins /etc/inittab.ins

