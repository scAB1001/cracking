# Cracking

## Ex-01 WALKTHROUGH

### View the code

View the ELF format in hexadecimal:
```bash
# Dump hex code to console
objdump -s  # --source-comment[=txt]
# Like the -S option, but all source code lines are displayed with a prefix of txt.
```

View the ELF format in ASM:
```bash
# Dump asm to file
objdump -d > login.asm
```

#### Locating the comparison

Identify the comparison:
```assembly
00000000000011a9 <main>:
    1217:	74 16                	je     122f <main+0x86>
```

Note the opcode, and memory address:
```text
opcode = 74
m.addr = 0x1217
```

Find the instruction in the x86 I.S. from https://shell-storm.org/x86doc/
```assembly
    JE rel8 	74 cb 		Jump short if equal (ZF=1).
```

Find inverse:
```assembly
    JNE rel8 	75 cb 		Jump short if not equal (ZF=0).
```

## Edit the code

```bash
# Install tool
sudo apt install hexedit

# Load object file into tool
hexedit login

# 1. Press enter and go to location 0x1217

# 2. Change 74 to 75

# 3. CTRL+X to Save & Exit

# Run the executable again
```