#!/bin/bash

# Script to set up FreeRTOS environment

# Step 1: Clone FreeRTOS Repository
echo "Cloning FreeRTOS repository into home directory..."
cd ~ || exit
if [ -d "FreeRTOS" ]; then
    echo "FreeRTOS directory already exists. Skipping clone."
else
    git clone https://github.com/FreeRTOS/FreeRTOS.git --recurse-submodules
fi

# Step 2: Install build essentials
echo "Installing build essentials..."
sudo apt update
sudo apt install -y build-essential

# Step 3: Add FREERTOS_PATH to .profile
echo "Adding FREERTOS_PATH to .profile..."
# if ! grep -q "export FREERTOS_PATH=~/FreeRTOS" ~/.profile; then
echo 'export FREERTOS_PATH=~/FreeRTOS' >> ~/.profile
echo "FREERTOS_PATH added to .profile"
# else
#     echo "FREERTOS_PATH already exists in .profile"
# fi

# Step 4: Source .profile
echo "Sourcing .profile to apply changes..."
source ~/.profile

# Step 5: Display FREERTOS_PATH
echo "FREERTOS_PATH is set to: $FREERTOS_PATH"
