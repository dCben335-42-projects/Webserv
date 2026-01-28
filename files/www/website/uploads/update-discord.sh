#!/bin/bash

mkdir -p /tmp/$USER
cd /tmp/$USER
rm -rf Discord discord.tar.gz discord-updater
wget "https://discord.com/api/download?platform=linux&format=tar.gz" -O discord.tar.gz
tar -xzf ./discord.tar.gz 
rm -rf discord.tar.gz
mkdir discord-updater
ln -s $PWD/Discord/Discord discord-updater/discord-updater
./discord-updater/discord-updater

