# Attitude

The `attitude` tool generates orientations to use for the calibration and distance modes of FOUND.

Optional Flags:
0. `--help`, `-h`: Help
1. `--use-local`: Use a given local orientation to generate a reference orientation for FOUND
2. `--local-attitude`: The local orientation to use with the flag above (must be used with above flag).
3. `--calibration-attitude`: The calibration attitude to use (describes transformation from reference to local frame)
4. `--num-attitude-pairs`: The number of attitude pairs to generate for the distance mode of FOUND

Example Usage (from `found` root):
```bash
>>> python3 -m tools.attitude --use-local --local-attitude 45 0 0 --num-attitude-pairs 3

========== CALIBRATION ATTITUDES ========== 

Calibration Attitude:     RA:  14.756, DE:  77.795, ROLL: 319.274 

Local Attitude:           RA: 272.236, DE: -29.259, ROLL:  37.154
Reference Attitude:       RA: 228.843, DE:  38.130, ROLL:  84.839 

Calibration Command:
./build/bin/found calibration \
--reference-orientation "228.842948936861 38.12967370616513 84.83945989677017" \
--local-orientation "272.23611078254874 -29.25898525400815 37.15440703313669" \
--output-file <calibration_file_name>.found 


========== DISTANCE ATTITUDES ========== 

===== TEST PAIR 1 ===== 

Local Attitude:           RA:  45.000, DE:  -0.000, ROLL:  -0.000
Reference Attitude:       RA: 317.190, DE: -75.585, ROLL:  49.943 

Generator Command:
tools.generator --position <Position> \
--orientation 45.00000000000001 -0.0 -0.0 \
<camera_parameters> \
--filename <generated_image_name> 

Distance Command:
./build/bin/found distance \
--calibration-data <calibration_file_name>.found \
--reference-orientation "317.1896836000723 -75.5846266781195 49.942682662774814" \
<camera_parameters> \
--image <generated_image_name> 


===== TEST PAIR 2 ===== 

Local Attitude:           RA: 268.440, DE:   3.362, ROLL:  81.185
Reference Attitude:       RA: 269.790, DE:  57.374, ROLL: 159.302 

Generator Command:
tools.generator --position <Position> \
--orientation 268.4403942879669 3.3617435714570316 81.18495536399615 \
<camera_parameters> \
--filename <generated_image_name> 

Distance Command:
./build/bin/found distance \
--calibration-data <calibration_file_name>.found \
--reference-orientation "269.79043976487196 57.37412986950127 159.30218566881078" \
<camera_parameters> \
--image <generated_image_name> 


===== TEST PAIR 3 ===== 

Local Attitude:           RA: 345.316, DE:  28.200, ROLL:  55.903
Reference Attitude:       RA: 309.672, DE: -12.132, ROLL: 128.372 

Generator Command:
tools.generator --position <Position> \
--orientation 345.315834301275 28.200499525315795 55.90281118134976 \
<camera_parameters> \
--filename <generated_image_name> 

Distance Command:
./build/bin/found distance \
--calibration-data <calibration_file_name>.found \
--reference-orientation "309.6719718956234 -12.13229325630242 128.3723252988373" \
<camera_parameters> \
--image <generated_image_name>
```