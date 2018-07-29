# r0: General Purpose
# r1: SCRIPT_STATUS register
#     bit 0: RUN
#     bit 1: POWER_BYTE   Current byte is a power byte (not step/dir)
# r2: CURRENT_BYTE / General Purpose
# r3: GPIO_VALUE / General Purpose
# r4: FIFO_MASK
# r5: LASER_PWM_ADDR
# r6: FIFO_BASE_ADDR
# r7: SDMA_CONTEXT_ADDR
#
#
# sc0 (24): X-POSITION
# sc1 (25): Y-POSITION
# sc2 (26): Z-POSITION
# sc3 (27): BYTE_COUNT
# sc4 (28): FIFO_HEAD
# sc5 (29): FIFO_TAIL
# sc6 (30): DIRECTION  0x00000001 = FORWARD, 0xFFFFFFFF = BACKWARD
# sc7 (31): STEP_COUNT
# ca  (18): STEP_DIR_GPIO_ADDR
# cs  (19): LASER_AUX_GPIO_ADDR
#
# Set by driver module:
# PDA (14): EPIT_STATUS_ADDR
# MDA (10): -
# MS  (12):  0x00000000  source and destination address frozen; start in read mode
# PS  (16):  0x000c0400  destination address frozen; 32-bit write size; start in write mode


###############################
# START
###############################
start:
	ldi	r1, 1           # Reset SCRIPT_STATUS register to RUN
	ldi	r3, 0		# Clear GPIO_VALUE
	stf	r1, 0xc8        # Clear EPIT Interrupt Flag (write a 0x1 to EPITSR)
	done	4               # HALT, and wait for EPIT event

###############################
# LOAD BYTE
###############################
# If we are starting from here, we received an EPIT event and we need to
# clear the EPIT Interrupt flag to restart the timer.  The timer ensures
# that our pulses are started on time.
fifo_consume_byte:
	stf r1, 0xc8        # Clear EPIT Interrupt Flag (write a 0x1 to EPITSR)

# If we are starting here, we processing another byte without restarting the EPIT timer.
# Perform some sanity checks on the pulse data.
fifo_consume_byte_no_iflag_clear:
	cmpeqi	r6, 0           # FIFO_BASE_ADDR == 0x00 ?
	bt	alldone         #   True -> alldone  Address not set, quit
	ld	r0, (r7, 28)    #
	and	r0, r4          # r0 = (FIFO_HEAD & FIFO_MASK)
	ld	r2, (r7, 29)    #
	and	r2, r4          # r2 = (FIFO_TAIL & FIFO_MASK)
	cmpeq	r0, r2          # r0 == r2 ?
	bt	alldone         #   True -> alldone  TAIL and HEAD are equal. No data to process.

# Read next byte from FIFO
	add	r0, r6          # r0 = FIFO_BASE_ADDR + (FIFO_HEAD & FIFO_MASK)
	stf	r0, 0x10        # r0 -> MSA Store next byte address in Memory Source Address register
	ldf	r2, 0x09        # MD -> r2  Load pulse byte from FIFO

# Check what kind of byte we read.
# If it's a POWER_BYTE, punt to the power byte handler
# For MOTION_BYTE, we check if we need to reverse direction.
	btsti	r2, 7           # Is this a POWER_BYTE or MOTION_BYTE ?
	bt	do_power_byte  	#   POWER_BYTE -> do_power_byte
	ld	r0, (r7, 30)    # sc6 (DIRECTION) -> r0
	btsti	r0, 31          # Reverse DIRECTION ?
	bf	do_dir	        #   False -> do_dir
	xori	r2, 0x4a        # We're running in reverse. Invert DIR bytes in MOTION_BYTE
	bclri	r2, 4           # Clear LASER_ON bit for reverse motion.

###############################
# PROCESS MOTION BYTE
###############################
# Set DIR outputs
do_dir:
	ld	r0, (r7, 18)    # ca (STEP_DIR_GPIO_ADDR) -> r0
	stf	r0, 0x14        # r0 (STEP_DIR_GPIO_ADDR) -> MDA, frozen mode
	ldi	r3, 0		# Clear GPIO_VALUE

# ---------- X-AXIS DIR ---------------
do_x_dir:
	bseti	r3, 14		# Set X_DIR HIGH
	btsti	r2, 1           # MOTION_BYTE X DIR LOW or HIGH ?
	bf	do_y_dir	#   BIT LOW, leave as is
	bclri	r3, 14		#   BIT HIGH, set X_DIR LOW
# ---------- Y-AXIS DIR ---------------
do_y_dir:
	bclri	r3, 2		# Set Y1_DIR LOW
	bseti	r3, 7		# Set Y2_DIR HIGH
	btsti	r2, 3           # MOTION_BYTE Y DIR LOW or HIGH ?
	bf	do_z_dir	#   BIT LOW, leave as is
	bseti	r3, 2		#   BIT HIGH, set Y1_DIR HIGH
	bclri	r3, 7		#   BIT HIGH, set Y2_DIR LOW
# ---------- Z-AXIS DIR --------------
do_z_dir:
	bclri	r3, 16		# Set Z_DIR LOW
	btsti	r2, 6           # MOTION_BYTE Z DIR LOW or HIGH ?
	bf	set_dir		#   BIT LOW, leave as is
	bseti	r3, 16		#   BIT HIGH, set Z_DIR HIGH
# Write DIR to GPIOs
set_dir:
	stf	r3, 0x2b	# r3 -> MD, Writes r3 to STEP_DIR_GPIO with immediate flush

# --------------------------------------
# Loop to hold DIR lines for time required by stepper drivers
	ldi	r0, 13
	loop	do_x_step, 0
	mov	r1, r1
	mov	r1, r1
	mov	r1, r1

# --------------------------------------
# Set STEP outputs
# ---------- X-AXIS STEP ---------------
do_x_step:
	btsti	r2, 0
	bf	do_y_step       # MOTION_BYTE X STEP ?
	bseti	r3, 15          #   True -> Set X_STEP output HIGH
# ---------- Y-AXIS STEP ---------------
do_y_step:
	btsti	r2, 2
	bf	do_z_step       # MOTION_BYTE Y STEP ?
	bseti	r3, 3           #   True -> Set Y1_STEP output HIGH
	bseti	r3, 9           #   True -> Set Y2_STEP output HIGH
# ---------- Z-AXIS STEP ---------------
do_z_step:
	btsti	r2, 5
	bf	set_steps       # MOTION_BYTE Z STEP ?
	bseti	r3, 11          #   True -> Set Z_STEP output HIGH
# Write STEPS to GPIOs
set_steps:
	stf	r3, 0x2b        # r3 -> MD, Writes r3 to STEP_DIR_GPIO with immediate flush

# --------------------------------------
# Set LASER outputs
# ---------- LASER ON/OFF ---------------
do_laser:
	ld	r0, (r7, 19)    # cs (LASER_AUX_GPIO_ADDR) -> r0
	stf	r0, 0x14        # r0 (LASER_AUX_GPIO_ADDR) -> MDA, frozen mode
	ldi	r3, 0		# Clear GPIO_VALUE
	btsti	r2, 4        	# MOTION_BYTE LASER ON ?
	bf	set_laser
	bseti	r3, 12		#   True -> Set FIRE HIGH
	bseti	r3, 0		#   True -> Set BEAM_DET_EN HIGH
# --------------------------------------
# Write LASER to GPIOs
set_laser:
	stf	r3, 0x2b        # r3 -> MD, Writes r3 to LASER_AUX_GPIO_ADDR with immediate flush

# --------------------------------------
# Loop to hold STEP lines for time required by stepper drivers
	ldi	r0, 50
	loop	upd_x_pos, 0
	mov	r1, r1
	mov	r1, r1
	mov	r1, r1

# ---------------------------------
# Update STEP_COUNTERS
# ---------- X-AXIS COUNT ---------
upd_x_pos:
	btsti	r2, 0           # If no X step, move on to Y
	bf	upd_y_pos
	ld	r0, (r7, 24)    # sc0 (X_POSITION) -> r0
	btsti	r2, 1
	bf	upd_x_pos2
	subi	r0, 2           # If negative direction, subtract 2 from position counter
upd_x_pos2:
	addi	r0, 1           # Add 1 to position counter, regardless of direction
	st	r0, (r7, 24)    # r0 -> sc0 (X_POSITION)
# ---------- Y-AXIS COUNT ---------
upd_y_pos:
	btsti	r2, 2           # If no Y step, move on to Z
	bf	upd_z_pos
	ld	r0, (r7, 25)    # sc1 (Y_POSITION) -> r0
	btsti	r2, 3
	bt	upd_y_pos2
	subi	r0, 2           # If negative direction, subtract 2 from position counter
upd_y_pos2:
	addi	r0, 1           # Add 1 to position counter, regardless of direction
	st	r0, (r7, 25)    # r0 -> sc0 (Y_POSITION)
# ---------- Z-AXIS COUNT ---------
upd_z_pos:
	btsti	r2, 5           # If no Z step, move to end of update
	bf	upd_pos_done
	ld	r0, (r7, 26)    # sc2 (Z_POSITION) -> r0
	btsti	r2, 6
	bt	upd_z_pos2
	subi	r0, 2           # If negative direction, subtract 2 from position counter
upd_z_pos2:
	addi	r0, 1           # Add 1 to position counter, regardless of direction
	st	r0, (r7, 26)    # r0 -> sc2 (Z_POSITION)
# ---------------------------------
upd_pos_done:
	bclri r1, 1           	# Clear SCRIPT_STATUS:POWER_BYTE flag

# ---------------------------------
# Clear STEP Bits
# -----------------------------------
clear_step_bits:
	ld	r0, (r7, 18)    # ca (STEP_DIR_GPIO_ADDR) -> r0
	stf	r0, 0x14        # r0 (STEP_DIR_GPIO_ADDR) -> MDA, frozen mode
	bclri	r3, 15          # X-STEP
	bclri	r3, 3           # Y1-STEP
	bclri	r3, 9           # Y2-STEP
	bclri	r3, 20          # Z-STEP
	stf	r3, 0x2b        # r3 -> MD, Writes r3 to STEP_DIR_GPIO_ADDR with immediate flush


###############################
# ADVANCE FIFO BYTE POSITION
###############################
# Update the byte counters
endbyte:
	ld	r2, (r7, 30)    # sc6 (DIRECTION) -> r2
	ld	r0, (r7, 27)    # sc3 (BYTE_COUNT) -> r0
	add	r0, r2          # DIRECTION + BYTE_COUNT -> r0
	st	r0, (r7, 27)    # Save new BYTE_COUNT
	ld	r0, (r7, 28)    # sc4 (FIFO_HEAD) -> r0
	add	r0, r2          # DIRECTION + FIFO_HEAD -> r0
	st	r0, (r7, 28)    # Save new FIFO_HEAD
	ld	r0, (r7, 31)    # sc7 (STEP_COUNT) -> r0
	cmpeqi	r0, 0
	bt	byte_done    	# If STEP_COUNT is already 0, head to byte_done
	subi	r0, 1           # STEP_COUNT = STEP_COUNT - 1
	st	r0, (r7, 31)    # STEP_COUNT -> sc7
	bf	byte_done    	# If new STEP_COUNT is not 0, head to byte_done
# ---------------------------------
# END OF BUFFER
	done	3               # We've ran the last STEP_COUNT.  Trigger INTERRUPT

# ---------------------------------
# BYTE COMPLETED
# Power Bytes don't count as a step, so we need to read another byte if the last
# byte was a power byte.
byte_done:
	btsti	r1, 2           # If SCRIPT_STATUS:POWER_BYTE flag is set,
	bclri	r1, 2           # clear the flag and process another byte.
	bt	fifo_consume_byte_no_iflag_clear
	done	4               # Otherwise, we HALT and wait for EPIT event
# !!! HALTED !!! WAITING ON EPIT EVENT
# ---
# EPIT event received
	btsti	r1, 0           # If SCRIPT_STATUS:RUN flag is set,
	bt	fifo_consume_byte  # True -> Consume next byte
alldone:
	done	3               # We've finished processing all bytes.  Halt and trigger INTERRUPT
# !!! HALTED !!! WAITING ON CHANEL START
# ---
# CHANEL START received
# We need to sync with the EPIT timer
	done	4               # HALT and wait for EPIT event
# !!! HALTED !!! WAITING ON EPIT EVENT
# ---
# EPIT event received
	btsti	r1, 0           # SCRIPT_STATUS:RUN flag set ?
	bt	start		#    True -> Start from the beginning
				# TODO: What happens if the RUN flag is not set?
				# Why do we process a power byte?


###############################
# PROCESS POWER BYTE
###############################
do_power_byte:
	btsti	r1, 1           # If SCRIPT_STATUS:POWER_BYTE, the last byte we processed was
	bt	endbyte         # was a POWER_BYTE, we'll ignore this one.
	ld	r0, (r7, 30)    # sc6 (DIRECTION) -> r0
	btsti	r0, 31          # If DIRECTION is reverse, skip setting the output
	bt	power_done
	ldf	r0, 0xd0        # PDA (holding EPIT_STATUS_ADDR) -> r0
	stf	r5, 0xd3        # r5 (LASER_PWM_ADDR) -> PDA
	andi	r2, 0x7f        # Strip uneeded bits from PWM setting
	stf	r2, 0xc8        # r2 -> PD, Writes r2 to LASER_PWM_ADDR with immediate flush
	stf	r0, 0xd3        # r0 (holding EPIT_STATUS_ADDR) -> PDA
	bseti	r1, 1           # Set SCRIPT_STATUS:POWER_BYTE flag
power_done:
	bseti	r1, 2
	btsti	r1, 0
	bt	endbyte         # If SCRIPT_STATUS:PROCESS_BYTE flag is set, head to endbyte.
