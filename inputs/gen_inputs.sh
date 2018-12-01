#!/bin/bash
echo "" > inputs.in
for i in {1..5}
do
   ./graphGen $i >> inputs.in
done