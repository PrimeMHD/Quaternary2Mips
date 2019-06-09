addi $sp,$sp,268697600
addi $fp,$fp,268697596
j lable_108
program:
subi $sp,$sp,4
sw $ra,0($sp)
subi $sp,$sp,4
sw $fp,0($sp)
move $fp,$sp
lw $t1,8($fp)
lw $t2,12($fp)
add $t1,$t1,$t2
move $t3,$t1
move $t4,$t3
move $v1,$t4
move $sp,$fp
addi $sp,$sp,16
lw $ra,4($fp)
lw $fp,0($fp)
jr $ra
lable_108:
main:
subi $sp,$sp,4
sw $ra,0($sp)
subi $sp,$sp,4
sw $fp,0($sp)
move $fp,$sp
addi $t1,$zero,1
move $t2,$t1
addi $t3,$zero,2
move $t4,$t3
addi $t5,$zero,3
move $t6,$t5
addi $t7,$zero,4
move $t8,$t7
subi $sp,$sp,4
sw $t2,($sp)
subi $sp,$sp,4
sw $t1,($sp)
jal program
move $t9,$v1
move $s0,$t5
addi $s1,$zero,360
move $s0,$s1
addi $s2,$zero,350
lable_125:
slt $s3,$s2,$s0
bnez $s3,lable_128
j lable_145
lable_128:
addi $t8,$t8,1
addi $s5,$zero,7
subu $s6,$t8,$s5
sltiu $s6,$s6,1
bnez $s6,lable_134
j lable_137
lable_134:
sw $t3,-12($fp)
add $t3,$s0,$t6
move $s0,$t3
j lable_145
lable_137:
sw $t4,-16($fp)
addi $t4,$zero,0
lable_138:
sw $t7,-28($fp)
slt $t7,$t4,$t6
sw $t2,-8($fp)
bnez $t7,lable_141
j lable_143
lable_141:
subi $t6,$t6,1
j lable_138
lable_143:
sub $s0,$s0,$t1
j lable_125
lable_145:
sw $t9,-36($fp)
move $t9,$t4
move $v1,$t9
move $sp,$fp
addi $sp,$sp,8
lw $ra,4($fp)
lw $fp,0($fp)
