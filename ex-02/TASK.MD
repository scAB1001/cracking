# 1. Make the user win immediately

0000000000001cf8 <update>:
    1dd9:	7c 11                	jl     1dec <update+0xf4>
JL rel8 	7C cb 		Jump short if less (SF≠ OF).

Inverse:
JG rel8 	7F cb 		Jump short if greater (ZF=0 and SF=OF).

# 2. Give the user a score of 999