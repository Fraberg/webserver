#!/bin/bash

echo "make fclean"
make fclean

echo "git add ."
git add .

echo "git commit -m $1"
git commit -m $1
