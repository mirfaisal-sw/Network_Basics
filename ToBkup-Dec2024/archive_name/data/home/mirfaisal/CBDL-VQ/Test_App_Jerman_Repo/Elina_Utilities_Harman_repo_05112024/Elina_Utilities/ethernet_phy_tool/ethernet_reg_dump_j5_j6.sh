#!/bin/sh

setup_j5e()
{
	# Addresses for J5e
	j5e_base=0x4A100000
	
	SS_BASE=$(($j5e_base + 0x0))
	PORT_BASE=$(($j5e_base + 0x0100))
	CPDMA_BASE=$(($j5e_base + 0x0800))
	STATS_BASE=$(($j5e_base + 0x0900))
	STATERAM_BASE=$(($j5e_base + 0x0a00))
	CPTS_BASE=$(($j5e_base + 0x0c00))
	ALE_BASE=$(($j5e_base + 0x0d00))
	SL1_BASE=$(($j5e_base + 0x0d80))
	SL2_BASE=$(($j5e_base + 0x0dc0))
	MDIO_BASE=$(($j5e_base + 0x01000))
	WR_BASE=$(($j5e_base + 0x01200))
	CPPI_RAM_BASE=$(($j5e_base + 0x02000))
	SPF1_BASE=0
	SPF2_BASE=0
}

setup_j6()
{
	# Addresses for J6
	SS_BASE=0x48484000
	PORT_BASE=0x48484100
	CPDMA_BASE=0x48484800
	STATS_BASE=0x48484900
	STATERAM_BASE=0x48484A00
	CPTS_BASE=0x48484C00
	ALE_BASE=0x48484D00
	SL1_BASE=0x48484D80
	SL2_BASE=0x48484DC0
	MDIO_BASE=0x48485000
	WR_BASE=0x48485200
	SPF1_BASE=0x48485C00
	SPF2_BASE=0x48485E00
}

function detect_variant
{
	rc=`cat /proc/cpuinfo | grep "DRA7"`
	if [ -n "$rc" ]; then
		hw_variant="j6"
	else
		rc=`cat /proc/cpuinfo | grep "DRA6"`
		if [ -n "$rc" ]; then
			hw_variant="j5e"
		fi
	fi
}

dump_stateram_tx()
{
	for ((channel=0; channel < 8 ; channel++))
	do
		TX_HDP=$(($STATERAM_BASE + 0 + 4 * $channel))
		TX_CP=$(($STATERAM_BASE + 0x40 + 4 * $channel))

		HDP=`devmem $TX_HDP`
		CP=`devmem $TX_CP`
		printf "Tx%d_HDP=0x%08x  TX%d_CP=0x%08x\n" $channel $HDP $channel $CP

		# dump through chain of buffer descriptors
		while [ $((HDP)) -ne 0 ]; 
		do
			next_buffer=`devmem $HDP`
			buffer_pointer=`devmem $(($HDP+4))`
			bol=`devmem $(($HDP+8))`
			flg=`devmem $(($HDP+0x0c))`
			buffer_length=$(($bol & 0x0000ffff))
			buffer_offset=$((($bol & 0xffff0000) >> 16))
			
			sop=$((($flg & 0x80000000) >> 31))
			eop=$((($flg & 0x40000000) >> 30))
			owner=$((($flg & 0x20000000) >> 29))
			eoq=$((($flg & 0x10000000) >> 28))
			TDOWNCMPLT=$((($flg & 0x08000000) >> 27))
			PASSCRC=$((($flg & 0x04000000) >> 26))
			TO_PORT_EN=$(($flg & 0x00100000))
			TO_PORT=$((($flg & 0x00030000) >> 16))
			packet_length=$(($flg & 0x000007ff))
			
			printf "      0x%08x: OWN=%d SOP=%d EOP=%d TO_PORT=%d  next_buffer=0x%08x\n" $HDP $owner $sop $eop $TO_PORT $next_buffer
			
			HDP=$next_buffer
		done
	done
}

dump_stateram_rx()
{
	for ((channel=0; channel < 8 ; channel++))
	do
		RX_HDP=$(($STATERAM_BASE + 0x20 + 4 * $channel))

		HDP=`devmem $RX_HDP`
		HDP_orig=$HDP
		
		printf "RX%d_HDP=0x%08x\n" $channel $HDP

		# dump through chain of buffer descriptors
		while [ $((HDP)) -ne 0 ]; 
		do
			next_buffer=`devmem $HDP`
			buffer_pointer=`devmem $(($HDP+4))`
			bol=`devmem $(($HDP+8))`
			flg=`devmem $(($HDP+0x0c))`
			buffer_length=$(($bol & 0x00000fff))
			buffer_offset=$((($bol & 0x0fff0000) >> 16))
			
			sop=$((($flg & 0x80000000) >> 31))
			eop=$((($flg & 0x40000000) >> 30))
			owner=$((($flg & 0x20000000) >> 29))
			eoq=$((($flg & 0x10000000) >> 28))
			TDOWNCMPLT=$((($flg & 0x08000000) >> 27))
			PASSCRC=$((($flg & 0x04000000) >> 26))
			LONG=$((($flg & 0x02000000) >> 25))
			SHORT=$((($flg & 0x01000000) >> 24))

			MAC_CTL=$((($flg & 0x00800000) >> 23))
			OVERRUN=$((($flg & 0x00400000) >> 22))
			PKT_ERR=$((($flg & 0x00300000) >> 20))
			RX_VLAN_ENCAP=$((($flg & 0x00080000) >> 19))
			FROM_PORT=$((($flg & 0x00070000) >> 16))
			packet_length=$(($flg & 0x000007ff))
			
			printf "      0x%08x: OWN=%d SOP=%d EOP=%d FROM_PORT=%d  packet_length=%d  next_buffer=0x%08x\n" $HDP $owner $sop $eop $FROM_PORT $packet_length $next_buffer
			
			HDP=$next_buffer
			if [ $((HDP_orig)) -eq $((HDP)) ]; then
				break
			fi
		done
	done
}

dump_cpdma()
{
	CPDMA_TX_IDVER=$(($CPDMA_BASE + 0x0))
	CPDMA_TX_CONTROL=$(($CPDMA_BASE + 0x04))
	CPDMA_TX_TEARDOWN=$(($CPDMA_BASE + 0x08))
	CPDMA_RX_IDVER=$(($CPDMA_BASE + 0x10))
	CPDMA_RX_CONTROL=$(($CPDMA_BASE + 0x014))
	CPDMA_RX_TEARDOWN=$(($CPDMA_BASE + 0x018))
	CPDMA_SOFT_RESET=$(($CPDMA_BASE + 0x01c))
	CPDMA_DMACONTROL=$(($CPDMA_BASE + 0x20))
	CPDMA_DMASTATUS=$(($CPDMA_BASE + 0x24))
	CPDMA_RX_BUFFER_OFFSET=$(($CPDMA_BASE + 0x28))
	CPDMA_EMCONTROL=$(($CPDMA_BASE + 0x2c))
	CPDMA_TX_PRI0_RATE=$(($CPDMA_BASE + 0x30))
	CPDMA_TX_PRI1_RATE=$(($CPDMA_BASE + 0x34))
	CPDMA_TX_PRI2_RATE=$(($CPDMA_BASE + 0x38))
	CPDMA_TX_PRI3_RATE=$(($CPDMA_BASE + 0x3c))
	CPDMA_TX_PRI4_RATE=$(($CPDMA_BASE + 0x40))
	CPDMA_TX_PRI5_RATE=$(($CPDMA_BASE + 0x44))
	CPDMA_TX_PRI6_RATE=$(($CPDMA_BASE + 0x48))
	CPDMA_TX_PRI7_RATE=$(($CPDMA_BASE + 0x4c))
	CPDMA_TX_INTSTAT_RAW=$(($CPDMA_BASE + 0x80))
	CPDMA_TX_INTSTAT_MASKED=$(($CPDMA_BASE + 0x84))
	CPDMA_TX_INTMASK_SET=$(($CPDMA_BASE + 0x88))
	CPDMA_TX_INTMASK_CLEAR=$(($CPDMA_BASE + 0x8c))
	CPDMA_IN_VECTOR=$(($CPDMA_BASE + 0x90))
	CPDMA_EOI_VECTOR=$(($CPDMA_BASE + 0x94))
	CPDMA_RX_INTSTAT_RAW=$(($CPDMA_BASE + 0xA0))
	CPDMA_RX_INTSTAT_MASKED=$(($CPDMA_BASE + 0xa4))
	CPDMA_RX_INTMASK_SET=$(($CPDMA_BASE + 0xa8))
	CPDMA_RX_INTMASK_CLEAR=$(($CPDMA_BASE + 0xac))
	CPDMA_DMA_INTSTAT_RAW=$(($CPDMA_BASE + 0xb0))
	CPDMA_DMA_INTSTAT_MASKED=$(($CPDMA_BASE + 0xb4))
	CPDMA_DMA_INTMASK_SET=$(($CPDMA_BASE + 0xb8))
	CPDMA_DMA_INTMASK_CLEAR=$(($CPDMA_BASE + 0xbc))
	CPDMA_RX0_PENDTHRESH=$(($CPDMA_BASE + 0xc0))
	CPDMA_RX1_PENDTHRESH=$(($CPDMA_BASE + 0xc4))
	CPDMA_RX2_PENDTHRESH=$(($CPDMA_BASE + 0xc8))
	CPDMA_RX3_PENDTHRESH=$(($CPDMA_BASE + 0xcc))
	CPDMA_RX4_PENDTHRESH=$(($CPDMA_BASE + 0xd0))
	CPDMA_RX5_PENDTHRESH=$(($CPDMA_BASE + 0xd4))
	CPDMA_RX6_PENDTHRESH=$(($CPDMA_BASE + 0xd8))
	CPDMA_RX7_PENDTHRESH=$(($CPDMA_BASE + 0xdc))
	CPDMA_RX0_FREEBUFFER=$(($CPDMA_BASE + 0xe0))
	CPDMA_RX1_FREEBUFFER=$(($CPDMA_BASE + 0xe4))
	CPDMA_RX2_FREEBUFFER=$(($CPDMA_BASE + 0xe8))
	CPDMA_RX3_FREEBUFFER=$(($CPDMA_BASE + 0xec))
	CPDMA_RX4_FREEBUFFER=$(($CPDMA_BASE + 0xf0))
	CPDMA_RX5_FREEBUFFER=$(($CPDMA_BASE + 0xf4))
	CPDMA_RX6_FREEBUFFER=$(($CPDMA_BASE + 0xf8))
	CPDMA_RX7_FREEBUFFER=$(($CPDMA_BASE + 0xfc))
	
	printf "==================CPDMA================================-\n"

	val=`devmem $CPDMA_TX_IDVER`

	printf "        CPDMA_TX_IDVER              0x%08x\n" $val
	printf "              REVISION              %d\n" $val
	
	val=`devmem $CPDMA_TX_CONTROL`
	TX_EN=$(($val & 0x00000001))
	
	printf "        CPDMA_TX_CONTROL            0x%08x\n" $val
	printf "              TX_EN                 %d\n" $TX_EN

	val=`devmem $CPDMA_TX_TEARDOWN`
	TX_TDN_RDY=$((($val & 0x80000000) >> 31))
	TX_TDN_CH=$(($val & 0x00000007))

	printf "        CPDMA_TX_TEARDOWN           0x%08x\n" $val
	printf "              TX_TDN_RDY            %u\n" $TX_TDN_RDY
	printf "              TX_TDN_CH             %u\n" $TX_TDN_CH

	val=`devmem $CPDMA_RX_IDVER`

	printf "        CPDMA_RX_IDVER              0x%08x\n" $val
	printf "              REVISION              %d\n" $val

	val=`devmem $CPDMA_RX_CONTROL`
	RX_EN=$(($val & 0x00000001))
	
	printf "        CPDMA_RX_CONTROL            0x%08x\n" $val
	printf "              RX_EN                 %d\n" $RX_EN

	val=`devmem $CPDMA_TX_TEARDOWN`
	RX_TDN_RDY=$((($val & 0x80000000) >> 31))
	RX_TDN_CH=$(($val & 0x00000007))

	printf "        CPDMA_TX_TEARDOWN           0x%08x\n" $val
	printf "              RX_TDN_RDY            %u\n" $RX_TDN_RDY
	printf "              RX_TDN_CH             %u\n" $RX_TDN_CH

	val=`devmem $CPDMA_SOFT_RESET`
	SOFT_RESET=$(($val & 0x00000001))
	
	printf "        CPDMA_SOFT_RESET            0x%08x\n" $val
	printf "              SOFT_RESET            %d\n" $RX_EN

	val=`devmem $CPDMA_DMACONTROL`
	TX_RLIM=$((($val & 0x0000ff00) >> 8))
	RX_CEF=$((($val & 0x00000010) >> 4))
	CMD_IDLE=$((($val & 0x00000008) >> 3))
	RX_OFFLEN_BLOCK=$((($val & 0x00000004) >> 2))
	RX_OWNERSHIP=$((($val & 0x00000002) >> 1))
	TX_PTYPE=$(($val & 0x00000001))
	
	printf "        CPDMA_DMACONTROL            0x%08x\n" $val
	printf "              TX_RLIM               %u\n" $TX_RLIM
	printf "              RX_CEF                %u\n" $RX_CEF
	printf "              CMD_IDLE              %u\n" $CMD_IDLE
	printf "              RX_OFFLEN_BLOCK       %u\n" $RX_OFFLEN_BLOCK
	printf "              RX_OWNERSHIP          %d\n" $RX_OWNERSHIP 
	printf "              TX_PTYPE              %d\n" $TX_PTYPE

	val=`devmem $CPDMA_DMASTATUS`
	IDLE=$((($val & 0x80000000) >> 31))
	TX_HOST_ERR_CODE=$((($val & 0x00F00000) >> 20))
	TX_ERR_CH=$((($val & 0x00070000) >> 16))
	RX_HOST_ERR_CODE=$((($val & 0x0000f000) >> 12))
	RX_ERR_CH=$((($val & 0x00000700) >> 8))
	
	printf "        CPDMA_DMASTATUS             0x%08x\n" $val
	printf "              IDLE                  %d\n" $IDLE
	printf "              TX_HOST_ERR_CODE      %d\n" $TX_HOST_ERR_CODE
	printf "              TX_ERR_CH             %d\n" $TX_ERR_CH
	printf "              RX_HOST_ERR_CODE      %d\n" $RX_HOST_ERR_CODE
	printf "              RX_ERR_CH             %d\n" $RX_ERR_CH

	val=`devmem $CPDMA_RX_BUFFER_OFFSET`
	RX_BUFFER_OFFSET=$(($val & 0x0000ffff))

	printf "        CPDMA_RX_BUFFER_OFFSET      0x%08x\n" $val
	printf "              RX_BUFFER_OFFSET      %u\n" $IDLE

	val=`devmem $CPDMA_EMCONTROL`
	SOFT=$((($val & 0x00000002) >> 1))
	FREE=$(($val & 0x00000001))
	
	printf "        CPDMA_EMCONTROL             0x%08x\n" $val
	printf "              SOFT                  %d\n" $SOFT
	printf "              FREE                  %d\n" $FREE

	val=`devmem $CPDMA_TX_PRI0_RATE`
	PRIN_IDLE_CNT=$((($val & 0x3fff0000) >> 16))
	PRIN_SEND_CNT=$(($val & 0x00003fff))
	
	printf "        CPDMA_TX_PRI0_RATE          0x%08x\n" $val
	printf "              PRIN_IDLE_CNT         %d\n" $PRIN_IDLE_CNT
	printf "              PRIN_SEND_CNT         %d\n" $PRIN_SEND_CNT

	val=`devmem $CPDMA_TX_PRI1_RATE`
	PRIN_IDLE_CNT=$((($val & 0x3fff0000) >> 16))
	PRIN_SEND_CNT=$(($val & 0x00003fff))
	
	printf "        CPDMA_TX_PRI1_RATE          0x%08x\n" $val
	printf "              PRIN_IDLE_CNT         %d\n" $PRIN_IDLE_CNT
	printf "              PRIN_SEND_CNT         %d\n" $PRIN_SEND_CNT

	val=`devmem $CPDMA_TX_PRI2_RATE`
	PRIN_IDLE_CNT=$((($val & 0x3fff0000) >> 16))
	PRIN_SEND_CNT=$(($val & 0x00003fff))
	
	printf "        CPDMA_TX_PRI2_RATE          0x%08x\n" $val
	printf "              PRIN_IDLE_CNT         %d\n" $PRIN_IDLE_CNT
	printf "              PRIN_SEND_CNT         %d\n" $PRIN_SEND_CNT

	val=`devmem $CPDMA_TX_PRI3_RATE`
	PRIN_IDLE_CNT=$((($val & 0x3fff0000) >> 16))
	PRIN_SEND_CNT=$(($val & 0x00003fff))
	
	printf "        CPDMA_TX_PRI3_RATE          0x%08x\n" $val
	printf "              PRIN_IDLE_CNT         %d\n" $PRIN_IDLE_CNT
	printf "              PRIN_SEND_CNT         %d\n" $PRIN_SEND_CNT

	val=`devmem $CPDMA_TX_PRI4_RATE`
	PRIN_IDLE_CNT=$((($val & 0x3fff0000) >> 16))
	PRIN_SEND_CNT=$(($val & 0x00003fff))
	
	printf "        CPDMA_TX_PRI4_RATE          0x%08x\n" $val
	printf "              PRIN_IDLE_CNT         %d\n" $PRIN_IDLE_CNT
	printf "              PRIN_SEND_CNT         %d\n" $PRIN_SEND_CNT

	val=`devmem $CPDMA_TX_PRI5_RATE`
	PRIN_IDLE_CNT=$((($val & 0x3fff0000) >> 16))
	PRIN_SEND_CNT=$(($val & 0x00003fff))
	
	printf "        CPDMA_TX_PRI5_RATE          0x%08x\n" $val
	printf "              PRIN_IDLE_CNT         %d\n" $PRIN_IDLE_CNT
	printf "              PRIN_SEND_CNT         %d\n" $PRIN_SEND_CNT

	val=`devmem $CPDMA_TX_PRI6_RATE`
	PRIN_IDLE_CNT=$((($val & 0x3fff0000) >> 16))
	PRIN_SEND_CNT=$(($val & 0x00003fff))
	
	printf "        CPDMA_TX_PRI6_RATE          0x%08x\n" $val
	printf "              PRIN_IDLE_CNT         %d\n" $PRIN_IDLE_CNT
	printf "              PRIN_SEND_CNT         %d\n" $PRIN_SEND_CNT

	val=`devmem $CPDMA_TX_PRI7_RATE`
	PRIN_IDLE_CNT=$((($val & 0x3fff0000) >> 16))
	PRIN_SEND_CNT=$(($val & 0x00003fff))
	
	printf "        CPDMA_TX_PRI7_RATE          0x%08x\n" $val
	printf "              PRIN_IDLE_CNT         %d\n" $PRIN_IDLE_CNT
	printf "              PRIN_SEND_CNT         %d\n" $PRIN_SEND_CNT

	val=`devmem $CPDMA_TX_INTSTAT_RAW`
	TX7_PEND=$((($val & 0x00000080) >> 7))
	TX6_PEND=$((($val & 0x00000040) >> 6))
	TX5_PEND=$((($val & 0x00000020) >> 5))
	TX4_PEND=$((($val & 0x00000010) >> 4))
	TX3_PEND=$((($val & 0x00000008) >> 3))
	TX2_PEND=$((($val & 0x00000004) >> 2))
	TX1_PEND=$((($val & 0x00000002) >> 1))
	TX0_PEND=$(($val & 0x00000001))
	
	printf "        CPDMA_TX_INTSTAT_RAW        0x%08x\n" $val
	printf "              TX7_PEND              %d\n" $TX7_PEND
	printf "              TX6_PEND              %d\n" $TX6_PEND
	printf "              TX5_PEND              %d\n" $TX5_PEND
	printf "              TX4_PEND              %d\n" $TX4_PEND
	printf "              TX3_PEND              %d\n" $TX3_PEND
	printf "              TX2_PEND              %d\n" $TX2_PEND
	printf "              TX2_PEND              %d\n" $TX1_PEND
	printf "              TX0_PEND              %d\n" $TX0_PEND

	val=`devmem $CPDMA_TX_INTSTAT_MASKED`
	TX7_PEND=$((($val & 0x00000080) >> 7))
	TX6_PEND=$((($val & 0x00000040) >> 6))
	TX5_PEND=$((($val & 0x00000020) >> 5))
	TX4_PEND=$((($val & 0x00000010) >> 4))
	TX3_PEND=$((($val & 0x00000008) >> 3))
	TX2_PEND=$((($val & 0x00000004) >> 2))
	TX1_PEND=$((($val & 0x00000002) >> 1))
	TX0_PEND=$(($val & 0x00000001))
	
	printf "        CPDMA_TX_INTSTAT_MASKED     0x%08x\n" $val
	printf "              TX7_PEND              %d\n" $TX7_PEND
	printf "              TX6_PEND              %d\n" $TX6_PEND
	printf "              TX5_PEND              %d\n" $TX5_PEND
	printf "              TX4_PEND              %d\n" $TX4_PEND
	printf "              TX3_PEND              %d\n" $TX3_PEND
	printf "              TX2_PEND              %d\n" $TX2_PEND
	printf "              TX2_PEND              %d\n" $TX1_PEND
	printf "              TX0_PEND              %d\n" $TX0_PEND

	val=`devmem $CPDMA_TX_INTMASK_SET`
	TX7_MASK=$((($val & 0x00000080) >> 7))
	TX6_MASK=$((($val & 0x00000040) >> 6))
	TX5_MASK=$((($val & 0x00000020) >> 5))
	TX4_MASK=$((($val & 0x00000010) >> 4))
	TX3_MASK=$((($val & 0x00000008) >> 3))
	TX2_MASK=$((($val & 0x00000004) >> 2))
	TX1_MASK=$((($val & 0x00000002) >> 1))
	TX0_MASK=$(($val & 0x00000001))
	
	printf "        CPDMA_TX_INTMASK_SET        0x%08x\n" $val
	printf "              TX7_MASK              %d\n" $TX7_MASK
	printf "              TX6_MASK              %d\n" $TX6_MASK
	printf "              TX5_MASK              %d\n" $TX5_MASK
	printf "              TX4_MASK              %d\n" $TX4_MASK
	printf "              TX3_MASK              %d\n" $TX3_MASK
	printf "              TX2_MASK              %d\n" $TX2_MASK
	printf "              TX2_MASK              %d\n" $TX1_MASK
	printf "              TX0_MASK              %d\n" $TX0_MASK

	val=`devmem $CPDMA_TX_INTMASK_CLEAR`
	TX7_MASK=$((($val & 0x00000080) >> 7))
	TX6_MASK=$((($val & 0x00000040) >> 6))
	TX5_MASK=$((($val & 0x00000020) >> 5))
	TX4_MASK=$((($val & 0x00000010) >> 4))
	TX3_MASK=$((($val & 0x00000008) >> 3))
	TX2_MASK=$((($val & 0x00000004) >> 2))
	TX1_MASK=$((($val & 0x00000002) >> 1))
	TX0_MASK=$(($val & 0x00000001))
	
	printf "        CPDMA_TX_INTMASK_CLEAR      0x%08x\n" $val
	printf "              TX7_MASK              %d\n" $TX7_MASK
	printf "              TX6_MASK              %d\n" $TX6_MASK
	printf "              TX5_MASK              %d\n" $TX5_MASK
	printf "              TX4_MASK              %d\n" $TX4_MASK
	printf "              TX3_MASK              %d\n" $TX3_MASK
	printf "              TX2_MASK              %d\n" $TX2_MASK
	printf "              TX2_MASK              %d\n" $TX1_MASK
	printf "              TX0_MASK              %d\n" $TX0_MASK

	val=`devmem $CPDMA_IN_VECTOR`

	printf "        CPDMA_IN_VECTOR             0x%08x\n" $val
	printf "              DMA_IN_VECTOR         %d\n" $val

	val=`devmem $CPDMA_EOI_VECTOR`
	DMA_EOI_VECTOR=$(($val & 0x0000001f))

	printf "        CPDMA_EOI_VECTOR            0x%08x\n" $val
	printf "              DMA_EOI_VECTOR        %d\n" $val

	val=`devmem $CPDMA_RX_INTSTAT_RAW`
	RX7_THRESH_PEND=$((($val & 0x00008000) >> 15))
	RX6_THRESH_PEND=$((($val & 0x00004000) >> 14))
	RX5_THRESH_PEND=$((($val & 0x00002000) >> 13))
	RX4_THRESH_PEND=$((($val & 0x00001000) >> 12))
	RX3_THRESH_PEND=$((($val & 0x00000800) >> 11))
	RX2_THRESH_PEND=$((($val & 0x00000400) >> 10))
	RX1_THRESH_PEND=$((($val & 0x00000200) >> 9))
	RX0_THRESH_PEND=$((($val & 0x00000100) >> 8))
	RX7_PEND=$((($val & 0x00000080) >> 7))
	RX6_PEND=$((($val & 0x00000040) >> 6))
	RX5_PEND=$((($val & 0x00000020) >> 5))
	RX4_PEND=$((($val & 0x00000010) >> 4))
	RX3_PEND=$((($val & 0x00000008) >> 3))
	RX2_PEND=$((($val & 0x00000004) >> 2))
	RX1_PEND=$((($val & 0x00000002) >> 1))
	RX0_PEND=$(($val & 0x00000001))
	
	printf "        CPDMA_RX_INTSTAT_RAW        0x%08x\n" $val
	printf "              RX7_THRESH_PEND       %d\n" $RX7_THRESH_PEND
	printf "              RX6_THRESH_PEND       %d\n" $RX6_THRESH_PEND
	printf "              RX5_THRESH_PEND       %d\n" $RX5_THRESH_PEND
	printf "              RX4_THRESH_PEND       %d\n" $RX4_THRESH_PEND
	printf "              RX3_THRESH_PEND       %d\n" $RX3_THRESH_PEND
	printf "              RX2_THRESH_PEND       %d\n" $RX2_THRESH_PEND
	printf "              RX1_THRESH_PEND       %d\n" $RX1_THRESH_PEND
	printf "              RX0_THRESH_PEND       %d\n" $RX0_THRESH_PEND
	printf "              TX7_PEND              %d\n" $RX7_PEND
	printf "              TX6_PEND              %d\n" $RX6_PEND
	printf "              TX5_PEND              %d\n" $RX5_PEND
	printf "              TX4_PEND              %d\n" $RX4_PEND
	printf "              TX3_PEND              %d\n" $RX3_PEND
	printf "              TX2_PEND              %d\n" $RX2_PEND
	printf "              TX2_PEND              %d\n" $RX1_PEND
	printf "              TX0_PEND              %d\n" $RX0_PEND

	val=`devmem $CPDMA_RX_INTSTAT_MASKED`
	RX7_THRESH_PEND=$((($val & 0x00008000) >> 15))
	RX6_THRESH_PEND=$((($val & 0x00004000) >> 14))
	RX5_THRESH_PEND=$((($val & 0x00002000) >> 13))
	RX4_THRESH_PEND=$((($val & 0x00001000) >> 12))
	RX3_THRESH_PEND=$((($val & 0x00000800) >> 11))
	RX2_THRESH_PEND=$((($val & 0x00000400) >> 10))
	RX1_THRESH_PEND=$((($val & 0x00000200) >> 9))
	RX0_THRESH_PEND=$((($val & 0x00000100) >> 8))
	RX7_PEND=$((($val & 0x00000080) >> 7))
	RX6_PEND=$((($val & 0x00000040) >> 6))
	RX5_PEND=$((($val & 0x00000020) >> 5))
	RX4_PEND=$((($val & 0x00000010) >> 4))
	RX3_PEND=$((($val & 0x00000008) >> 3))
	RX2_PEND=$((($val & 0x00000004) >> 2))
	RX1_PEND=$((($val & 0x00000002) >> 1))
	RX0_PEND=$(($val & 0x00000001))
	
	printf "        CPDMA_RX_INTSTAT_MASKED     0x%08x\n" $val
	printf "              RX7_THRESH_PEND       %d\n" $RX7_THRESH_PEND
	printf "              RX6_THRESH_PEND       %d\n" $RX6_THRESH_PEND
	printf "              RX5_THRESH_PEND       %d\n" $RX5_THRESH_PEND
	printf "              RX4_THRESH_PEND       %d\n" $RX4_THRESH_PEND
	printf "              RX3_THRESH_PEND       %d\n" $RX3_THRESH_PEND
	printf "              RX2_THRESH_PEND       %d\n" $RX2_THRESH_PEND
	printf "              RX1_THRESH_PEND       %d\n" $RX1_THRESH_PEND
	printf "              RX0_THRESH_PEND       %d\n" $RX0_THRESH_PEND
	printf "              TX7_PEND              %d\n" $RX7_PEND
	printf "              TX6_PEND              %d\n" $RX6_PEND
	printf "              TX5_PEND              %d\n" $RX5_PEND
	printf "              TX4_PEND              %d\n" $RX4_PEND
	printf "              TX3_PEND              %d\n" $RX3_PEND
	printf "              TX2_PEND              %d\n" $RX2_PEND
	printf "              TX2_PEND              %d\n" $RX1_PEND
	printf "              TX0_PEND              %d\n" $RX0_PEND

	val=`devmem $CPDMA_RX_INTMASK_SET`
	RX7_THRESH_PEND_MASK=$((($val & 0x00008000) >> 15))
	RX6_THRESH_PEND_MASK=$((($val & 0x00004000) >> 14))
	RX5_THRESH_PEND_MASK=$((($val & 0x00002000) >> 13))
	RX4_THRESH_PEND_MASK=$((($val & 0x00001000) >> 12))
	RX3_THRESH_PEND_MASK=$((($val & 0x00000800) >> 11))
	RX2_THRESH_PEND_MASK=$((($val & 0x00000400) >> 10))
	RX1_THRESH_PEND_MASK=$((($val & 0x00000200) >> 9))
	RX0_THRESH_PEND_MASK=$((($val & 0x00000100) >> 8))
	RX7_PEND_MASK=$((($val & 0x00000080) >> 7))
	RX6_PEND_MASK=$((($val & 0x00000040) >> 6))
	RX5_PEND_MASK=$((($val & 0x00000020) >> 5))
	RX4_PEND_MASK=$((($val & 0x00000010) >> 4))
	RX3_PEND_MASK=$((($val & 0x00000008) >> 3))
	RX2_PEND_MASK=$((($val & 0x00000004) >> 2))
	RX1_PEND_MASK=$((($val & 0x00000002) >> 1))
	RX0_PEND_MASK=$(($val & 0x00000001))
	
	printf "        CPDMA_RX_INTMASK_SET        0x%08x\n" $val
	printf "              RX7_THRESH_PEND_MASK  %d\n" $RX7_THRESH_PEND_MASK
	printf "              RX6_THRESH_PEND_MASK  %d\n" $RX6_THRESH_PEND_MASK
	printf "              RX5_THRESH_PEND_MASK  %d\n" $RX5_THRESH_PEND_MASK
	printf "              RX4_THRESH_PEND_MASK  %d\n" $RX4_THRESH_PEND_MASK
	printf "              RX3_THRESH_PEND_MASK  %d\n" $RX3_THRESH_PEND_MASK
	printf "              RX2_THRESH_PEND_MASK  %d\n" $RX2_THRESH_PEND_MASK
	printf "              RX1_THRESH_PEND_MASK  %d\n" $RX1_THRESH_PEND_MASK
	printf "              RX0_THRESH_PEND_MASK  %d\n" $RX0_THRESH_PEND_MASK
	printf "              TX7_PEND_MASK         %d\n" $RX7_PEND_MASK
	printf "              TX6_PEND_MASK         %d\n" $RX6_PEND_MASK
	printf "              TX5_PEND_MASK         %d\n" $RX5_PEND_MASK
	printf "              TX4_PEND_MASK         %d\n" $RX4_PEND_MASK
	printf "              TX3_PEND_MASK         %d\n" $RX3_PEND_MASK
	printf "              TX2_PEND_MASK         %d\n" $RX2_PEND_MASK
	printf "              TX2_PEND_MASK         %d\n" $RX1_PEND_MASK
	printf "              TX0_PEND_MASK         %d\n" $RX0_PEND_MASK

	val=`devmem $CPDMA_RX_INTMASK_CLEAR`
	RX7_THRESH_PEND_MASK=$((($val & 0x00008000) >> 15))
	RX6_THRESH_PEND_MASK=$((($val & 0x00004000) >> 14))
	RX5_THRESH_PEND_MASK=$((($val & 0x00002000) >> 13))
	RX4_THRESH_PEND_MASK=$((($val & 0x00001000) >> 12))
	RX3_THRESH_PEND_MASK=$((($val & 0x00000800) >> 11))
	RX2_THRESH_PEND_MASK=$((($val & 0x00000400) >> 10))
	RX1_THRESH_PEND_MASK=$((($val & 0x00000200) >> 9))
	RX0_THRESH_PEND_MASK=$((($val & 0x00000100) >> 8))
	RX7_PEND_MASK=$((($val & 0x00000080) >> 7))
	RX6_PEND_MASK=$((($val & 0x00000040) >> 6))
	RX5_PEND_MASK=$((($val & 0x00000020) >> 5))
	RX4_PEND_MASK=$((($val & 0x00000010) >> 4))
	RX3_PEND_MASK=$((($val & 0x00000008) >> 3))
	RX2_PEND_MASK=$((($val & 0x00000004) >> 2))
	RX1_PEND_MASK=$((($val & 0x00000002) >> 1))
	RX0_PEND_MASK=$(($val & 0x00000001))
	
	printf "        CPDMA_RX_INTMASK_CLEAR      0x%08x\n" $val
	printf "              RX7_THRESH_PEND_MASK  %d\n" $RX7_THRESH_PEND_MASK
	printf "              RX6_THRESH_PEND_MASK  %d\n" $RX6_THRESH_PEND_MASK
	printf "              RX5_THRESH_PEND_MASK  %d\n" $RX5_THRESH_PEND_MASK
	printf "              RX4_THRESH_PEND_MASK  %d\n" $RX4_THRESH_PEND_MASK
	printf "              RX3_THRESH_PEND_MASK  %d\n" $RX3_THRESH_PEND_MASK
	printf "              RX2_THRESH_PEND_MASK  %d\n" $RX2_THRESH_PEND_MASK
	printf "              RX1_THRESH_PEND_MASK  %d\n" $RX1_THRESH_PEND_MASK
	printf "              RX0_THRESH_PEND_MASK  %d\n" $RX0_THRESH_PEND_MASK
	printf "              TX7_PEND_MASK         %d\n" $RX7_PEND_MASK
	printf "              TX6_PEND_MASK         %d\n" $RX6_PEND_MASK
	printf "              TX5_PEND_MASK         %d\n" $RX5_PEND_MASK
	printf "              TX4_PEND_MASK         %d\n" $RX4_PEND_MASK
	printf "              TX3_PEND_MASK         %d\n" $RX3_PEND_MASK
	printf "              TX2_PEND_MASK         %d\n" $RX2_PEND_MASK
	printf "              TX2_PEND_MASK         %d\n" $RX1_PEND_MASK
	printf "              TX0_PEND_MASK         %d\n" $RX0_PEND_MASK

	val=`devmem $CPDMA_DMA_INTSTAT_RAW`
	HOST_PEND=$((($val & 0x00000002) >> 1))
	STAT_PEND=$(($val & 0x00000001))

	printf "        CPDMA_DMA_INTSTAT_RAW       0x%08x\n" $val
	printf "              HOST_PEND             %d\n" $HOST_PEND
	printf "              STAT_PEND             %d\n" $STAT_PEND

	val=`devmem $CPDMA_DMA_INTSTAT_MASKED`
	HOST_PEND=$((($val & 0x00000002) >> 1))
	STAT_PEND=$(($val & 0x00000001))

	printf "        CPDMA_DMA_INTSTAT_MASKED    0x%08x\n" $val
	printf "              HOST_PEND             %d\n" $HOST_PEND
	printf "              STAT_PEND             %d\n" $STAT_PEND

	val=`devmem $CPDMA_DMA_INTMASK_SET`
	HOST_ERR_INT_MASK=$((($val & 0x00000002) >> 1))
	STAT_INT_MASK=$(($val & 0x00000001))

	printf "        CPDMA_DMA_INTMASK_SET       0x%08x\n" $val
	printf "              HOST_ERR_INT_MASK     %d\n" $HOST_ERR_INT_MASK
	printf "              STAT_INT_MASK         %d\n" $STAT_INT_MASK

	val=`devmem $CPDMA_DMA_INTMASK_CLEAR`
	HOST_ERR_INT_MASK=$((($val & 0x00000002) >> 1))
	STAT_INT_MASK=$(($val & 0x00000001))

	printf "        CPDMA_DMA_INTMASK_CLEAR     0x%08x\n" $val
	printf "              HOST_ERR_INT_MASK     %d\n" $HOST_ERR_INT_MASK
	printf "              STAT_INT_MASK         %d\n" $STAT_INT_MASK

	val=`devmem $CPDMA_RX0_PENDTHRESH`
	RX_PENDTHRESH=$(($val & 0x000000ff))

	printf "        CPDMA_RX0_PENDTHRESH        0x%08x\n" $val
	printf "              RX_PENDTHRESH         %d\n" $RX_PENDTHRESH

	val=`devmem $CPDMA_RX1_PENDTHRESH`
	RX_PENDTHRESH=$(($val & 0x000000ff))

	printf "        CPDMA_RX1_PENDTHRESH        0x%08x\n" $val
	printf "              RX_PENDTHRESH         %d\n" $RX_PENDTHRESH

	val=`devmem $CPDMA_RX2_PENDTHRESH`
	RX_PENDTHRESH=$(($val & 0x000000ff))

	printf "        CPDMA_RX2_PENDTHRESH        0x%08x\n" $val
	printf "              RX_PENDTHRESH         %d\n" $RX_PENDTHRESH

	val=`devmem $CPDMA_RX3_PENDTHRESH`
	RX_PENDTHRESH=$(($val & 0x000000ff))

	printf "        CPDMA_RX3_PENDTHRESH        0x%08x\n" $val
	printf "              RX_PENDTHRESH         %d\n" $RX_PENDTHRESH

	val=`devmem $CPDMA_RX4_PENDTHRESH`
	RX_PENDTHRESH=$(($val & 0x000000ff))

	printf "        CPDMA_RX4_PENDTHRESH        0x%08x\n" $val
	printf "              RX_PENDTHRESH         %d\n" $RX_PENDTHRESH

	val=`devmem $CPDMA_RX5_PENDTHRESH`
	RX_PENDTHRESH=$(($val & 0x000000ff))

	printf "        CPDMA_RX5_PENDTHRESH        0x%08x\n" $val
	printf "              RX_PENDTHRESH         %d\n" $RX_PENDTHRESH

	val=`devmem $CPDMA_RX6_PENDTHRESH`
	RX_PENDTHRESH=$(($val & 0x000000ff))

	printf "        CPDMA_RX6_PENDTHRESH        0x%08x\n" $val
	printf "              RX_PENDTHRESH         %d\n" $RX_PENDTHRESH

	val=`devmem $CPDMA_RX7_PENDTHRESH`
	RX_PENDTHRESH=$(($val & 0x000000ff))

	printf "        CPDMA_RX7_PENDTHRESH        0x%08x\n" $val
	printf "              RX_PENDTHRESH         %d\n" $RX_PENDTHRESH

	val=`devmem $CPDMA_RX0_FREEBUFFER`
	RX_FREEBUFFER=$(($val & 0x0000ffff))

	printf "        CPDMA_RX0_FREEBUFFER        0x%08x\n" $val
	printf "              RX_FREEBUFFER         %d\n" $RX_FREEBUFFER

	val=`devmem $CPDMA_RX1_FREEBUFFER`
	RX_FREEBUFFER=$(($val & 0x0000ffff))

	printf "        CPDMA_RX1_FREEBUFFER        0x%08x\n" $val
	printf "              RX_FREEBUFFER         %d\n" $RX_FREEBUFFER

	val=`devmem $CPDMA_RX2_FREEBUFFER`
	RX_FREEBUFFER=$(($val & 0x0000ffff))

	printf "        CPDMA_RX2_FREEBUFFER        0x%08x\n" $val
	printf "              RX_FREEBUFFER         %d\n" $RX_FREEBUFFER

	val=`devmem $CPDMA_RX3_FREEBUFFER`
	RX_FREEBUFFER=$(($val & 0x0000ffff))

	printf "        CPDMA_RX3_FREEBUFFER        0x%08x\n" $val
	printf "              RX_FREEBUFFER         %d\n" $RX_FREEBUFFER

	val=`devmem $CPDMA_RX4_FREEBUFFER`
	RX_FREEBUFFER=$(($val & 0x0000ffff))

	printf "        CPDMA_RX4_FREEBUFFER        0x%08x\n" $val
	printf "              RX_FREEBUFFER         %d\n" $RX_FREEBUFFER

	val=`devmem $CPDMA_RX5_FREEBUFFER`
	RX_FREEBUFFER=$(($val & 0x0000ffff))

	printf "        CPDMA_RX5_FREEBUFFER        0x%08x\n" $val
	printf "              RX_FREEBUFFER         %d\n" $RX_FREEBUFFER

	val=`devmem $CPDMA_RX6_FREEBUFFER`
	RX_FREEBUFFER=$(($val & 0x0000ffff))

	printf "        CPDMA_RX6_FREEBUFFER        0x%08x\n" $val
	printf "              RX_FREEBUFFER         %d\n" $RX_FREEBUFFER

	val=`devmem $CPDMA_RX7_FREEBUFFER`
	RX_FREEBUFFER=$(($val & 0x0000ffff))

	printf "        CPDMA_RX7_FREEBUFFER        0x%08x\n" $val
	printf "              RX_FREEBUFFER         %d\n" $RX_FREEBUFFER

	printf "========================================================\n"
}

dump_stats()
{
	GOOD_RX_FRAMES=$((STATS_BASE + 0x0))
	BROADCAST_RX_FRAMES=$((STATS_BASE + 0x4))
	MULTICAST_RX_FRAMES=$((STATS_BASE + 0x8))
	PAUSE_RX_FRAMES=$((STATS_BASE + 0xc))
	RX_CRC_ERRORS=$((STATS_BASE + 0x10))
	RX_ALIGN_CODE_ERRORS=$((STATS_BASE + 0x14))
	OVERSIZE_RX_FRAMES=$((STATS_BASE + 0x18))
	RX_JABBERS=$((STATS_BASE + 0x1c))
	UNDERSIZE_RX_FRAMES=$((STATS_BASE + 0x20))
	RX_FRAGMENTS=$((STATS_BASE + 0x24))
	RX_OCTETS=$((STATS_BASE + 0x30))

	GOOD_TX_FRAMES=$((STATS_BASE + 0x034))
	BROADCAST_TX_FRAMES=$((STATS_BASE + 0x038))
	MULTICAST_TX_FRAMES=$((STATS_BASE + 0x03c))
	PAUSE_TX_FRAMES=$((STATS_BASE + 0x40))
	DEFERRED_TX_FRAMES=$((STATS_BASE + 0x044))
	COLLISIONS=$((STATS_BASE + 0x048))
	SINGLE_COLLISION_TX_FRAMES=$((STATS_BASE + 0x04c))
	MULTIPLE_COLLISION_TX_FRAMES=$((STATS_BASE + 0x50))
	EXCESSIVE_COLLISIONS=$((STATS_BASE + 0x054))
	LATE_COLLISIONS=$((STATS_BASE + 0x058))
	TX_UNDERRUN=$((STATS_BASE + 0x05c))
	CARRIER_SENSE_ERRORS=$((STATS_BASE + 0x60))
	TX_OCTETS=$((STATS_BASE + 0x064))
	RX_TX_64_OCTET_FRAMES=$((STATS_BASE + 0x068))
	RX_TX_65_127_OCTET_FRAMES=$((STATS_BASE + 0x06c))
	RX_TX_128_255_OCTET_FRAMES=$((STATS_BASE + 0x70))
	RX_TX_256_511_OCTET_FRAMES=$((STATS_BASE + 0x074))
	RX_TX_512_1023_OCTET_FRAMES=$((STATS_BASE + 0x078))
	RX_TX_1024_UP_OCTET_FRAMES=$((STATS_BASE + 0x07c))
	NET_OCTETS=$((STATS_BASE + 0x80))
	RX_START_OF_FRAME_OVERRUNS=$((STATS_BASE + 0x084))
	RX_MIDDLE_OF_FRAME_OVERRUNS=$((STATS_BASE + 0x088))
	RX_DMA_OVERRUNS=$((STATS_BASE + 0x08c))
	
	GOOD_RX_FRAMES_VAL=`devmem $GOOD_RX_FRAMES`
	BROADCAST_RX_FRAMES_VAL=`devmem $BROADCAST_RX_FRAMES`
	MULTICAST_RX_FRAMES_VAL=`devmem $MULTICAST_RX_FRAMES`
	PAUSE_RX_FRAMES_VAL=`devmem $PAUSE_RX_FRAMES`
	RX_CRC_ERRORS_VAL=`devmem $RX_CRC_ERRORS`
	RX_ALIGN_CODE_ERRORS_VAL=`devmem $RX_ALIGN_CODE_ERRORS`
	OVERSIZE_RX_FRAMES_VAL=`devmem $OVERSIZE_RX_FRAMES`
	RX_JABBERS_VAL=`devmem $RX_JABBERS`
	UNDERSIZE_RX_FRAMES_VAL=`devmem $UNDERSIZE_RX_FRAMES`
	RX_FRAGMENTS_VAL=`devmem $RX_FRAGMENTS`
	RX_OCTETS_VAL=`devmem $RX_OCTETS`

	GOOD_TX_FRAMES_VAL=`devmem $GOOD_TX_FRAMES`
	BROADCAST_TX_FRAMES_VAL=`devmem $BROADCAST_TX_FRAMES`
	MULTICAST_TX_FRAMES_VAL=`devmem $MULTICAST_TX_FRAMES`
	PAUSE_TX_FRAMES_VAL=`devmem $PAUSE_TX_FRAMES`
	DEFERRED_TX_FRAMES_VAL=`devmem $DEFERRED_TX_FRAMES`
	COLLISIONS_VAL=`devmem $COLLISIONS`
	SINGLE_COLLISION_TX_FRAMES_VAL=`devmem $SINGLE_COLLISION_TX_FRAMES`
	MULTIPLE_COLLISION_TX_FRAMES_VAL=`devmem $MULTIPLE_COLLISION_TX_FRAMES`
	EXCESSIVE_COLLISIONS_VAL=`devmem $EXCESSIVE_COLLISIONS`
	LATE_COLLISIONS_VAL=`devmem $LATE_COLLISIONS`
	TX_UNDERRUN_VAL=`devmem $TX_UNDERRUN`
	CARRIER_SENSE_ERRORS_VAL=`devmem $CARRIER_SENSE_ERRORS`
	TX_OCTETS_VAL=`devmem $TX_OCTETS`
	RX_TX_64_OCTET_FRAMES_VAL=`devmem $RX_TX_64_OCTET_FRAMES`
	RX_TX_65_127_OCTET_FRAMES_VAL=`devmem $RX_TX_65_127_OCTET_FRAMES`
	RX_TX_128_255_OCTET_FRAMES_VAL=`devmem $RX_TX_128_255_OCTET_FRAMES`
	RX_TX_256_511_OCTET_FRAMES_VAL=`devmem $RX_TX_256_511_OCTET_FRAMES`
	RX_TX_512_1023_OCTET_FRAMES_VAL=`devmem $RX_TX_512_1023_OCTET_FRAMES`
	RX_TX_1024_UP_OCTET_FRAMES_VAL=`devmem $RX_TX_1024_UP_OCTET_FRAMES`
	NET_OCTETS_VAL=`devmem $NET_OCTETS`
	RX_START_OF_FRAME_OVERRUNS_VAL=`devmem $RX_START_OF_FRAME_OVERRUNS`
	RX_MIDDLE_OF_FRAME_OVERRUNS_VAL=`devmem $RX_MIDDLE_OF_FRAME_OVERRUNS`
	RX_DMA_OVERRUNS_VAL=`devmem $RX_DMA_OVERRUNS`
	
	printf "==================STATS================================-\n"

	printf "        GOOD_RX_FRAMES               %10u\n" $GOOD_RX_FRAMES_VAL
	printf "        BROADCAST_RX_FRAMES          %10u\n" $BROADCAST_RX_FRAMES_VAL
	printf "        MULTICAST_RX_FRAMES          %10u\n" $MULTICAST_RX_FRAMES_VAL
	printf "        PAUSE_RX_FRAMES              %10u\n" $PAUSE_RX_FRAMES_VAL
	printf "        RX_CRC_ERRORS                %10u\n" $RX_CRC_ERRORS_VAL
	printf "        RX_ALIGN_CODE_ERRORS         %10u\n" $RX_ALIGN_CODE_ERRORS_VAL
	printf "        OVERSIZE_RX_FRAMES           %10u\n" $OVERSIZE_RX_FRAMES_VAL
	printf "        RX_JABBERS                   %10u\n" $RX_JABBERS_VAL
	printf "        UNDERSIZE_RX_FRAMES          %10u\n" $UNDERSIZE_RX_FRAMES_VAL
	printf "        RX_FRAGMENT                  %10u\n" $RX_FRAGMENTS_VAL
	printf "        RX_OCTETS                    %10u\n" $RX_OCTETS_VAL

	printf "        GOOD_TX_FRAMES               %10u\n" $GOOD_TX_FRAMES_VAL
	printf "        BROADCAST_TX_FRAMES          %10u\n" $BROADCAST_TX_FRAMES_VAL
	printf "        MULTICAST_TX_FRAMES          %10u\n" $MULTICAST_TX_FRAMES_VAL
	printf "        PAUSE_TX_FRAMES              %10u\n" $PAUSE_TX_FRAMES_VAL
	printf "        DEFERRED_TX_FRAMES           %10u\n" $DEFERRED_TX_FRAMES_VAL
	printf "        COLLISIONS                   %10u\n" $COLLISIONS_VAL
	printf "        SINGLE_COLLISION_TX_FRAMES   %10u\n" $SINGLE_COLLISION_TX_FRAMES_VAL
	printf "        MULTIPLE_COLLISION_TX_FRAMES %10u\n" $MULTIPLE_COLLISION_TX_FRAMES_VAL
	printf "        EXCESSIVE_COLLISIONS         %10u\n" $EXCESSIVE_COLLISIONS_VAL
	printf "        LATE_COLLISIONS              %10u\n" $LATE_COLLISIONS_VAL
	printf "        TX_UNDERRUN                  %10u\n" $TX_UNDERRUN_VAL
	printf "        CARRIER_SENSE_ERRORS         %10u\n" $CARRIER_SENSE_ERRORS_VAL
	printf "        TX_OCTETS                    %10u\n" $TX_OCTETS_VAL
	printf "        RX_TX_64_OCTET_FRAMES        %10u\n" $RX_TX_64_OCTET_FRAMES_VAL
	printf "        RX_TX_65_127_OCTET_FRAMES    %10u\n" $RX_TX_65_127_OCTET_FRAMES_VAL
	printf "        RX_TX_128_255_OCTET_FRAMES   %10u\n" $RX_TX_128_255_OCTET_FRAMES_VAL
	printf "        RX_TX_256_511_OCTET_FRAMES   %10u\n" $RX_TX_256_511_OCTET_FRAMES_VAL
	printf "        RX_TX_512_1023_OCTET_FRAMES  %10u\n" $RX_TX_512_1023_OCTET_FRAMES_VAL
	printf "        RX_TX_1024_UP_OCTET_FRAMES   %10u\n" $RX_TX_1024_UP_OCTET_FRAMES_VAL
	printf "        NET_OCTETS                   %10u\n" $NET_OCTETS_VAL
	printf "        RX_START_OF_FRAME_OVERRUNS   %10u\n" $RX_START_OF_FRAME_OVERRUNS_VAL
	printf "        RX_MIDDLE_OF_FRAME_OVERRUNS  %10u\n" $RX_MIDDLE_OF_FRAME_OVERRUNS_VAL
	printf "        RX_DMA_OVERRUNS              %10u\n" $RX_DMA_OVERRUNS_VAL

	printf "========================================================\n"
}

dump_ss()
{
	CPSW_ID_VER=$((SS_BASE + 0x0))
	CPSW_CONTROL=$((SS_BASE + 0x04))
	CPSW_SOFT_RESET=$((SS_BASE + 0x08))
	CPSW_STAT_PORT_EN=$((SS_BASE + 0x0c))
	CPSW_PTYPE=$((SS_BASE + 0x0))
	CPSW_SOFT_IDLE=$((SS_BASE + 0x014))
	CPSW_THRU_RATE=$((SS_BASE + 0x018))
	CPSW_GAP_THRESH=$((SS_BASE + 0x01c))
	CPSW_TX_START_WDS=$((SS_BASE + 0x20))
	CPSW_FLOW_CONTROL=$((SS_BASE + 0x024))
	CPSW_VLAN_LTYPE=$((SS_BASE + 0x028))
	CPSW_TS_LTYPE=$((SS_BASE + 0x02c))
	CPSW_DLR_LTYPE=$((SS_BASE + 0x30))
	CPSW_EEE_PRESCALE=$((SS_BASE + 0x034))

	CPSW_ID_VER_VAL=`devmem $CPSW_ID_VER`
	CPSW_CONTROL_VAL=`devmem $CPSW_CONTROL`
	EEE_EN=$((($CPSW_CONTROL_VAL & 0x00000010) >> 4))
	DLR_EN=$((($CPSW_CONTROL_VAL & 0x00000008) >> 3))
	RX_VLAN_ENCAP=$((($CPSW_CONTROL_VAL & 0x00000004) >> 2))
	VLAN_AWARE=$((($CPSW_CONTROL_VAL & 0x00000002) >> 1))
	FIFO_LOOPBACK=$(($CPSW_CONTROL_VAL & 0x00000001))

	CPSW_SOFT_RESET_VAL=`devmem $CPSW_SOFT_RESET`
	SOFT_RESET=$(($CPSW_SOFT_RESET_VAL & 0x00000001))
	
	CPSW_STAT_PORT_EN_VAL=`devmem $CPSW_STAT_PORT_EN`
	P2_STAT_EN=$((($CPSW_STAT_PORT_EN_VAL & 0x00000004) >> 2))
	P1_STAT_EN=$((($CPSW_STAT_PORT_EN_VAL & 0x00000002) >> 1))
	P0_STAT_EN=$(($CPSW_STAT_PORT_EN_VAL & 0x00000001))
	
	CPSW_PTYPE_VAL=`devmem $CPSW_PTYPE`
	P2_PRI3_SHAPE_EN=$((($CPSW_PTYPE_VAL & 0x00200000) >> 21))
	P2_PRI2_SHAPE_EN=$((($CPSW_PTYPE_VAL & 0x00100000) >> 20))
	P2_PRI1_SHAPE_EN=$((($CPSW_PTYPE_VAL & 0x00080000) >> 19))
	P1_PRI3_SHAPE_EN=$((($CPSW_PTYPE_VAL & 0x00040000) >> 18))
	P1_PRI2_SHAPE_EN=$((($CPSW_PTYPE_VAL & 0x00020000) >> 17))
	P1_PRI1_SHAPE_EN=$((($CPSW_PTYPE_VAL & 0x00010000) >> 16))
	P2_PTYPE_ESC=$((($CPSW_PTYPE_VAL & 0x00000400) >> 10))
	P1_PTYPE_ESC=$((($CPSW_PTYPE_VAL & 0x00000200) >> 9))
	P0_PTYPE_ESC=$((($CPSW_PTYPE_VAL & 0x00000100) >> 8))
	ESC_PRI_LD_VAL=$(($CPSW_PTYPE_VAL & 0x0000001f))
	
	CPSW_SOFT_IDLE_VAL=`devmem $CPSW_SOFT_IDLE`
	SOFT_IDLE=$(($CPSW_SOFT_IDLE_VAL & 0x00000001))
	
	CPSW_THRU_RATE_VAL=`devmem $CPSW_THRU_RATE`
	SL_RX_THRU_RATE=$((($CPSW_THRU_RATE_VAL & 0x0000f000) >> 12))
	CPDMA_THRU_RATE=$(($CPSW_THRU_RATE_VAL & 0x0000000f))
	
	CPSW_GAP_THRESH_VAL=`devmem $CPSW_GAP_THRESH`
	GAP_THRESH=$(($CPSW_GAP_THRESH_VAL & 0x0000001f))
	
	CPSW_TX_START_WDS_VAL=`devmem $CPSW_TX_START_WDS`
	TX_START_WDS=$(($CPSW_TX_START_WDS_VAL & 0x000007ff))
	
	CPSW_FLOW_CONTROL_VAL=`devmem $CPSW_FLOW_CONTROL`
	P2_FLOW_EN=$((($CPSW_FLOW_CONTROL_VAL & 0x00000004) >> 2))
	P1_FLOW_EN=$((($CPSW_FLOW_CONTROL_VAL & 0x00000004) >> 2))
	P0_FLOW_EN=$(($CPSW_FLOW_CONTROL_VAL & 0x00000001))
	
	CPSW_VLAN_LTYPE_VAL=`devmem $CPSW_VLAN_LTYPE`
	VLAN_LTYPE2=$((($CPSW_VLAN_LTYPE_VAL & 0xffff00000) >> 16))
	VLAN_LTYPE1=$(($CPSW_VLAN_LTYPE_VAL & 0x0000ffff))
	
	CPSW_TS_LTYPE_VAL=`devmem $CPSW_TS_LTYPE`
	TS_LTYPE2=$((($CPSW_TS_LTYPE_VAL & 0xffff00000) >> 16))
	TS_LTYPE1=$(($CPSW_TS_LTYPE_VAL & 0x0000ffff))
	
	CPSW_DLR_LTYPE_VAL=`devmem $CPSW_DLR_LTYPE`
	DLR_LTYPE=$(($CPSW_DLR_LTYPE_VAL & 0x0000ffff))
	
	CPSW_EEE_PRESCALE_VAL=`devmem $CPSW_EEE_PRESCALE`
	EEE_PRESCALE=$(($CPSW_EEE_PRESCALE_VAL & 0x00000fff))

	printf "====================-SS================================-\n"

	printf "        CPSW_ID_VER\n"
	printf "              REVISION         %u\n" $CPSW_ID_VER_VAL
	printf "        CPSW_CONTROL\n"
	printf "              EEE_EN           %u\n" $EEE_EN
	printf "              DLR_EN           %u\n" $DLR_EN
	printf "              RX_VLAN_ENCAP    %u\n" $RX_VLAN_ENCAP
	printf "              VLAN_AWARE       %u\n" $VLAN_AWARE
	printf "              FIFO_LOOPBACK    %u\n" $FIFO_LOOPBACK
	printf "        CPSW_SOFT_RESET\n"
	printf "              SOFT_RESET       %u\n" $SOFT_RESET
	printf "        CPSW_STAT_PORT_EN\n"
	printf "              P2_STAT_EN       %u\n" $P2_STAT_EN
	printf "              P1_STAT_EN       %u\n" $P1_STAT_EN
	printf "              P0_STAT_EN       %u\n" $P0_STAT_EN
	printf "        CPSW_PTYPE\n"
	printf "              P2_PRI3_SHAPE_EN %u\n" $P2_PRI3_SHAPE_EN
	printf "              P2_PRI2_SHAPE_EN %u\n" $P2_PRI2_SHAPE_EN
	printf "              P2_PRI1_SHAPE_EN %u\n" $P2_PRI1_SHAPE_EN
	printf "              P1_PRI3_SHAPE_EN %u\n" $P1_PRI3_SHAPE_EN
	printf "              P1_PRI2_SHAPE_EN %u\n" $P1_PRI2_SHAPE_EN
	printf "              P1_PRI1_SHAPE_EN %u\n" $P1_PRI1_SHAPE_EN
	printf "              P2_PTYPE_ESC     %u\n" $P2_PTYPE_ESC
	printf "              P1_PTYPE_ESC     %u\n" $P1_PTYPE_ESC
	printf "              P0_PTYPE_ESC     %u\n" $P0_PTYPE_ESC
	printf "              ESC_PRI_LD_VAL   %u\n" $ESC_PRI_LD_VAL
	printf "        CPSW_SOFT_IDLE\n"
	printf "              SOFT_IDLE        %u\n" $SOFT_IDLE
	printf "        CPSW_THRU_RATE\n"
	printf "              SL_RX_THRU_RATE  %u\n" $SL_RX_THRU_RATE
	printf "              CPDMA_THRU_RATE  %u\n" $CPDMA_THRU_RATE
	printf "        CPSW_GAP_THRESH\n"
	printf "              GAP_THRESH       %u\n" $GAP_THRESH
	printf "        CPSW_TX_START_WDS\n"
	printf "              TX_START_WDS     %u\n" $TX_START_WDS
	printf "        CPSW_FLOW_CONTROL\n"
	printf "              P2_FLOW_EN      %u\n" $P2_FLOW_EN
	printf "              P1_FLOW_EN      %u\n" $P1_FLOW_EN
	printf "              P0_FLOW_EN      %u\n" $P0_FLOW_EN
	printf "        CPSW_VLAN_LTYPE\n"
	printf "              VLAN_LTYPE2     0x%04x\n" $VLAN_LTYPE2
	printf "              VLAN_LTYPE1     0x%04x\n" $VLAN_LTYPE1
	printf "        CPSW_TS_LTYPE\n"
	printf "              TS_LTYPE2       0x%04x\n" $TS_LTYPE2
	printf "              TS_LTYPE1       0x%04x\n" $TS_LTYPE1
	printf "        CPSW_DLR_LTYPE\n"
	printf "              DLR_LTYPE       0x%04x\n" $DLR_LTYPE
	printf "        CPSW_EEE_PRESCALE\n"
	printf "              EEE_PRESCALE    %u\n" $EEE_PRESCALE

	printf "========================================================\n"
}

dump_port()
{
	P0_CONTROL=$((PORT_BASE + 0x0))
	P0_CONTROL_VAL=`devmem $P0_CONTROL`
	P0_DLR_CPDMA_CH=$((($P0_CONTROL_VAL & 0x70000000) >> 28))
	P0_PASS_PRI_TAGGED=$((($P0_CONTROL_VAL & 0x01000000) >> 24))
	P0_VLAN_LTYPE2_EN=$((($P0_CONTROL_VAL & 0x00200000) >> 21))
	P0_VLAN_LTYPE1_EN=$((($P0_CONTROL_VAL & 0x00100000) >> 20))
	P0_DSCP_PRI_EN=$((($P0_CONTROL_VAL & 0x00010000) >> 16))
	
	P0_MAX_BLKS=$((PORT_BASE + 0x08))
	P0_MAX_BLKS_VAL=`devmem $P0_MAX_BLKS`
	P0_TX_MAX_BLKS=$((($P0_MAX_BLKS_VAL & 0x000001f0) >> 4))
	P0_RX_MAX_BLKS=$(($P0_MAX_BLKS_VAL & 0x0000000f))
	
	P0_BLK_CNT=$((PORT_BASE + 0x0c))
	P0_BLK_CNT_VAL=`devmem $P0_BLK_CNT`
	P0_TX_BLK_CNT=$((($P0_BLK_CNT_VAL & 0x000001f0) >> 4))
	P0_RX_MAX_BLKS=$(($P0_BLK_CNT_VAL & 0x0000000f))
	
	P0_TX_IN_CTL=$((PORT_BASE + 0x10))
	P0_TX_IN_CTL_VAL=`devmem $P0_TX_IN_CTL`
	P0_TX_RATE_EN=$((($P0_TX_IN_CTL_VAL & 0x00f00000) >> 20))
	P0_TX_IN_SEL=$((($P0_TX_IN_CTL_VAL & 0x00030000) >> 16))
	P0_TX_BLKS_REM=$((($P0_TX_IN_CTL_VAL & 0x0000f000) >> 12))
	P0_TX_PRI_WDS=$(($P0_TX_IN_CTL_VAL & 0x000003ff))
	
	P0_PORT_VLAN=$((PORT_BASE + 0x014))
	P0_PORT_VLAN_VAL=`devmem $P0_PORT_VLAN`
	P0_PORT_PRI=$((($P0_PORT_VLAN_VAL & 0x0000e000) >> 13))
	P0_PORT_CFI=$((($P0_PORT_VLAN_VAL & 0x00001000) >> 12))
	P0_PORT_VID=$(($P0_PORT_VLAN_VAL & 0x00000fff))
	
	P0_TX_PRI_MAP=$((PORT_BASE + 0x018))
	P0_TX_PRI_MAP_VAL=`devmem $P0_TX_PRI_MAP`
	P0_PRI7=$((($P0_TX_PRI_MAP_VAL & 0x30000000) >> 28))
	P0_PRI6=$((($P0_TX_PRI_MAP_VAL & 0x03000000) >> 24))
	P0_PRI5=$((($P0_TX_PRI_MAP_VAL & 0x00300000) >> 20))
	P0_PRI4=$((($P0_TX_PRI_MAP_VAL & 0x00030000) >> 16))
	P0_PRI3=$((($P0_TX_PRI_MAP_VAL & 0x00003000) >> 12))
	P0_PRI2=$((($P0_TX_PRI_MAP_VAL & 0x00000300) >> 8))
	P0_PRI1=$((($P0_TX_PRI_MAP_VAL & 0x00000030) >> 4))
	P0_PRI0=$(($P0_TX_PRI_MAP_VAL & 0x00000003))
	
	P0_CPDMA_TX_PRI_MAP=$((PORT_BASE + 0x01c))
	P0_CPDMA_TX_PRI_MAP_VAL=`devmem $P0_CPDMA_TX_PRI_MAP`
	P0_CPDMA_TX_PRI7=$((($P0_CPDMA_TX_PRI_MAP_VAL & 0x30000000) >> 28))
	P0_CPDMA_TX_PRI6=$((($P0_CPDMA_TX_PRI_MAP_VAL & 0x03000000) >> 24))
	P0_CPDMA_TX_PRI5=$((($P0_CPDMA_TX_PRI_MAP_VAL & 0x00300000) >> 20))
	P0_CPDMA_TX_PRI4=$((($P0_CPDMA_TX_PRI_MAP_VAL & 0x00030000) >> 16))
	P0_CPDMA_TX_PRI3=$((($P0_CPDMA_TX_PRI_MAP_VAL & 0x00003000) >> 12))
	P0_CPDMA_TX_PRI2=$((($P0_CPDMA_TX_PRI_MAP_VAL & 0x00000300) >> 8))
	P0_CPDMA_TX_PRI1=$((($P0_CPDMA_TX_PRI_MAP_VAL & 0x00000030) >> 4))
	P0_CPDMA_TX_PRI0=$(($P0_CPDMA_TX_PRI_MAP_VAL & 0x00000003))
	
	P0_CPDMA_RX_CH_MAP=$((PORT_BASE + 0x20))
	P0_CPDMA_RX_CH_MAP_VAL=`devmem $P0_CPDMA_RX_CH_MAP`
	P2_PRI3=$((($P0_CPDMA_RX_CH_MAP_VAL & 0x70000000) >> 28))
	P2_PRI2=$((($P0_CPDMA_RX_CH_MAP_VAL & 0x07000000) >> 24))
	P2_PRI1=$((($P0_CPDMA_RX_CH_MAP_VAL & 0x00700000) >> 20))
	P2_PRI0=$((($P0_CPDMA_RX_CH_MAP_VAL & 0x00070000) >> 16))
	P1_PRI3=$((($P0_CPDMA_RX_CH_MAP_VAL & 0x00007000) >> 12))
	P1_PRI2=$((($P0_CPDMA_RX_CH_MAP_VAL & 0x00000700) >> 8))
	P1_PRI1=$((($P0_CPDMA_RX_CH_MAP_VAL & 0x00000070) >> 4))
	P1_PRI0=$(($P0_CPDMA_RX_CH_MAP_VAL & 0x00000007))
	
	P0_RX_DSCP_PRI_MAP0=$((PORT_BASE + 0x30))
	P0_RX_DSCP_PRI_MAP0_VAL=`devmem $P0_RX_DSCP_PRI_MAP0`
	P0_RX_PRI7=$((($P0_RX_DSCP_PRI_MAP0_VAL & 0x70000000) >> 28))
	P0_RX_PRI6=$((($P0_RX_DSCP_PRI_MAP0_VAL & 0x07000000) >> 24))
	P0_RX_PRI5=$((($P0_RX_DSCP_PRI_MAP0_VAL & 0x00700000) >> 20))
	P0_RX_PRI4=$((($P0_RX_DSCP_PRI_MAP0_VAL & 0x00070000) >> 16))
	P0_RX_PRI3=$((($P0_RX_DSCP_PRI_MAP0_VAL & 0x00007000) >> 12))
	P0_RX_PRI2=$((($P0_RX_DSCP_PRI_MAP0_VAL & 0x00000700) >> 8))
	P0_RX_PRI1=$((($P0_RX_DSCP_PRI_MAP0_VAL & 0x00000070) >> 4))
	P0_RX_PRI0=$(($P0_RX_DSCP_PRI_MAP0_VAL & 0x00000007))

	P0_RX_DSCP_PRI_MAP1=$((PORT_BASE + 0x034))
	P0_RX_DSCP_PRI_MAP1_VAL=`devmem $P0_RX_DSCP_PRI_MAP1`
	P0_RX_PRI15=$((($P0_RX_DSCP_PRI_MAP1_VAL & 0x70000000) >> 28))
	P0_RX_PRI14=$((($P0_RX_DSCP_PRI_MAP1_VAL & 0x07000000) >> 24))
	P0_RX_PRI13=$((($P0_RX_DSCP_PRI_MAP1_VAL & 0x00700000) >> 20))
	P0_RX_PRI12=$((($P0_RX_DSCP_PRI_MAP1_VAL & 0x00070000) >> 16))
	P0_RX_PRI11=$((($P0_RX_DSCP_PRI_MAP1_VAL & 0x00007000) >> 12))
	P0_RX_PRI10=$((($P0_RX_DSCP_PRI_MAP1_VAL & 0x00000700) >> 8))
	P0_RX_PRI9=$((($P0_RX_DSCP_PRI_MAP1_VAL & 0x00000070) >> 4))
	P0_RX_PRI8=$(($P0_RX_DSCP_PRI_MAP1_VAL & 0x00000007))
	
	P0_RX_DSCP_PRI_MAP2=$((PORT_BASE + 0x038))
	P0_RX_DSCP_PRI_MAP2_VAL=`devmem $P0_RX_DSCP_PRI_MAP2`
	P0_RX_PRI23=$((($P0_RX_DSCP_PRI_MAP2_VAL & 0x70000000) >> 28))
	P0_RX_PRI22=$((($P0_RX_DSCP_PRI_MAP2_VAL & 0x07000000) >> 24))
	P0_RX_PRI21=$((($P0_RX_DSCP_PRI_MAP2_VAL & 0x00700000) >> 20))
	P0_RX_PRI20=$((($P0_RX_DSCP_PRI_MAP2_VAL & 0x00070000) >> 16))
	P0_RX_PRI19=$((($P0_RX_DSCP_PRI_MAP2_VAL & 0x00007000) >> 12))
	P0_RX_PRI18=$((($P0_RX_DSCP_PRI_MAP2_VAL & 0x00000700) >> 8))
	P0_RX_PRI17=$((($P0_RX_DSCP_PRI_MAP2_VAL & 0x00000070) >> 4))
	P0_RX_PRI16=$(($P0_RX_DSCP_PRI_MAP2_VAL & 0x00000007))
	
	P0_RX_DSCP_PRI_MAP3=$((PORT_BASE + 0x03c))
	P0_RX_DSCP_PRI_MAP3_VAL=`devmem $P0_RX_DSCP_PRI_MAP3`
	P0_RX_PRI31=$((($P0_RX_DSCP_PRI_MAP3_VAL & 0x70000000) >> 28))
	P0_RX_PRI30=$((($P0_RX_DSCP_PRI_MAP3_VAL & 0x07000000) >> 24))
	P0_RX_PRI29=$((($P0_RX_DSCP_PRI_MAP3_VAL & 0x00700000) >> 20))
	P0_RX_PRI28=$((($P0_RX_DSCP_PRI_MAP3_VAL & 0x00070000) >> 16))
	P0_RX_PRI27=$((($P0_RX_DSCP_PRI_MAP3_VAL & 0x00007000) >> 12))
	P0_RX_PRI26=$((($P0_RX_DSCP_PRI_MAP3_VAL & 0x00000700) >> 8))
	P0_RX_PRI25=$((($P0_RX_DSCP_PRI_MAP3_VAL & 0x00000070) >> 4))
	P0_RX_PRI24=$(($P0_RX_DSCP_PRI_MAP3_VAL & 0x00000007))
	
	P0_RX_DSCP_PRI_MAP4=$((PORT_BASE + 0x40))
	P0_RX_DSCP_PRI_MAP4_VAL=`devmem $P0_RX_DSCP_PRI_MAP4`
	P0_RX_PRI39=$((($P0_RX_DSCP_PRI_MAP4_VAL & 0x70000000) >> 28))
	P0_RX_PRI38=$((($P0_RX_DSCP_PRI_MAP4_VAL & 0x07000000) >> 24))
	P0_RX_PRI37=$((($P0_RX_DSCP_PRI_MAP4_VAL & 0x00700000) >> 20))
	P0_RX_PRI36=$((($P0_RX_DSCP_PRI_MAP4_VAL & 0x00070000) >> 16))
	P0_RX_PRI35=$((($P0_RX_DSCP_PRI_MAP4_VAL & 0x00007000) >> 12))
	P0_RX_PRI34=$((($P0_RX_DSCP_PRI_MAP4_VAL & 0x00000700) >> 8))
	P0_RX_PRI33=$((($P0_RX_DSCP_PRI_MAP4_VAL & 0x00000070) >> 4))
	P0_RX_PRI32=$(($P0_RX_DSCP_PRI_MAP4_VAL & 0x00000007))
	
	P0_RX_DSCP_PRI_MAP5=$((PORT_BASE + 0x044))
	P0_RX_DSCP_PRI_MAP5_VAL=`devmem $P0_RX_DSCP_PRI_MAP5`
	P0_RX_PRI47=$((($P0_RX_DSCP_PRI_MAP5_VAL & 0x70000000) >> 28))
	P0_RX_PRI46=$((($P0_RX_DSCP_PRI_MAP5_VAL & 0x07000000) >> 24))
	P0_RX_PRI45=$((($P0_RX_DSCP_PRI_MAP5_VAL & 0x00700000) >> 20))
	P0_RX_PRI44=$((($P0_RX_DSCP_PRI_MAP5_VAL & 0x00070000) >> 16))
	P0_RX_PRI43=$((($P0_RX_DSCP_PRI_MAP5_VAL & 0x00007000) >> 12))
	P0_RX_PRI42=$((($P0_RX_DSCP_PRI_MAP5_VAL & 0x00000700) >> 8))
	P0_RX_PRI41=$((($P0_RX_DSCP_PRI_MAP5_VAL & 0x00000070) >> 4))
	P0_RX_PRI40=$(($P0_RX_DSCP_PRI_MAP5_VAL & 0x00000007))
	
	P0_RX_DSCP_PRI_MAP6=$((PORT_BASE + 0x048))
	P0_RX_DSCP_PRI_MAP6_VAL=`devmem $P0_RX_DSCP_PRI_MAP6`
	P0_RX_PRI55=$((($P0_RX_DSCP_PRI_MAP6_VAL & 0x70000000) >> 28))
	P0_RX_PRI54=$((($P0_RX_DSCP_PRI_MAP6_VAL & 0x07000000) >> 24))
	P0_RX_PRI53=$((($P0_RX_DSCP_PRI_MAP6_VAL & 0x00700000) >> 20))
	P0_RX_PRI52=$((($P0_RX_DSCP_PRI_MAP6_VAL & 0x00070000) >> 16))
	P0_RX_PRI51=$((($P0_RX_DSCP_PRI_MAP6_VAL & 0x00007000) >> 12))
	P0_RX_PRI50=$((($P0_RX_DSCP_PRI_MAP6_VAL & 0x00000700) >> 8))
	P0_RX_PRI49=$((($P0_RX_DSCP_PRI_MAP6_VAL & 0x00000070) >> 4))
	P0_RX_PRI48=$(($P0_RX_DSCP_PRI_MAP6_VAL & 0x00000007))
	
	P0_RX_DSCP_PRI_MAP7=$((PORT_BASE + 0x04c))
	P0_RX_DSCP_PRI_MAP7_VAL=`devmem $P0_RX_DSCP_PRI_MAP7`
	P0_RX_PRI63=$((($P0_RX_DSCP_PRI_MAP7_VAL & 0x70000000) >> 28))
	P0_RX_PRI62=$((($P0_RX_DSCP_PRI_MAP7_VAL & 0x07000000) >> 24))
	P0_RX_PRI61=$((($P0_RX_DSCP_PRI_MAP7_VAL & 0x00700000) >> 20))
	P0_RX_PRI60=$((($P0_RX_DSCP_PRI_MAP7_VAL & 0x00070000) >> 16))
	P0_RX_PRI59=$((($P0_RX_DSCP_PRI_MAP7_VAL & 0x00007000) >> 12))
	P0_RX_PRI58=$((($P0_RX_DSCP_PRI_MAP7_VAL & 0x00000700) >> 8))
	P0_RX_PRI57=$((($P0_RX_DSCP_PRI_MAP7_VAL & 0x00000070) >> 4))
	P0_RX_PRI56=$(($P0_RX_DSCP_PRI_MAP7_VAL & 0x00000007))
	
	P0_IDLE2LPI=$((PORT_BASE + 0x50))
	P0_IDLE2LPI_VAL=`devmem $P0_IDLE2LPI`
	P0_IDLE2LPI_VAL=$(($P0_IDLE2LPI_VAL & 0x000fffff))

	P0_LPI2WAKE=$((PORT_BASE + 0x054))
	P0_LPI2WAKE_VAL=`devmem $P0_LPI2WAKE`
	P0_LPI2WAKE_VAL=$(($P0_LPI2WAKE_VAL & 0x000fffff))
	
	P1_CONTROL=$((PORT_BASE + 0x100))
	P1_CONTROL_VAL=`devmem $P1_CONTROL`
	P1_TX_CLKSTOP_EN=$((($P1_CONTROL_VAL   & 0x02000000) >> 25))
	P1_PASS_PRI_TAGGED=$((($P1_CONTROL_VAL & 0x01000000) >> 24))
	P1_VLAN_LTYPE2_EN=$((($P1_CONTROL_VAL & 0x00200000) >> 21))
	P1_VLAN_LTYPE1_EN=$((($P1_CONTROL_VAL & 0x00100000) >> 20))
	P1_DSCP_PRI_EN=$((($P1_CONTROL_VAL & 0x00010000) >> 16))
	P1_TS_107=$((($P1_CONTROL_VAL & 0x00008000) >> 15))
	P1_TS_320=$((($P1_CONTROL_VAL & 0x00004000) >> 14))
	P1_TS_319=$((($P1_CONTROL_VAL & 0x00002000) >> 13))
	P1_TS_132=$((($P1_CONTROL_VAL & 0x00001000) >> 12))
	P1_TS_131=$((($P1_CONTROL_VAL & 0x00000800) >> 11))
	P1_TS_130=$((($P1_CONTROL_VAL & 0x00000400) >> 10))
	P1_TS_129=$((($P1_CONTROL_VAL & 0x00000200) >> 9))
	P1_TS_TTL_NONZERO=$((($P1_CONTROL_VAL & 0x00000100) >> 8))
	P1_TS_UNI_EN=$((($P1_CONTROL_VAL & 0x00000080) >> 7))
	P1_TS_ANNEX_F_EN=$((($P1_CONTROL_VAL & 0x00000040) >> 6))
	P1_TS_ANNEX_E_EN=$((($P1_CONTROL_VAL & 0x00000020) >> 5))
	P1_TS_ANNEX_D_EN=$((($P1_CONTROL_VAL & 0x00000010) >> 4))
	P1_TS_LTYPE2_EN=$((($P1_CONTROL_VAL & 0x00000008) >> 3))
	P1_TS_LTYPE1_EN=$((($P1_CONTROL_VAL & 0x00000004) >> 2))
	P1_TS_TX_EN=$((($P1_CONTROL_VAL & 0x00000002) >> 1))
	P1_TS_RX_EN=$(($P1_CONTROL_VAL & 0x00000001))

	P1_MAX_BLKS=$((PORT_BASE + 0x108))
	P1_MAX_BLKS_VAL=`devmem $P1_MAX_BLKS`
	P1_TX_MAX_BLKS=$((($P1_MAX_BLKS_VAL & 0x000001f0) >> 4))
	P1_RX_MAX_BLKS=$(($P1_MAX_BLKS_VAL & 0x0000000f))
	
	P1_BLK_CNT=$((PORT_BASE + 0x10c))
	P1_BLK_CNT_VAL=`devmem $P1_BLK_CNT`
	P1_TX_BLK_CNT=$((($P1_BLK_CNT_VAL & 0x000001f0) >> 4))
	P1_RX_MAX_BLKS=$(($P1_BLK_CNT_VAL & 0x0000000f))
	
	P1_TX_IN_CTL=$((PORT_BASE + 0x110))
	P1_TX_IN_CTL_VAL=`devmem $P1_TX_IN_CTL`
	P1_HOST_BLKS_REM=$((($P1_TX_IN_CTL_VAL & 0x0f000000) >> 24))
	P1_TX_RATE_EN=$((($P1_TX_IN_CTL_VAL & 0x00f00000) >> 20))
	P1_TX_IN_SEL=$((($P1_TX_IN_CTL_VAL & 0x00030000) >> 16))
	P1_TX_BLKS_REM=$((($P1_TX_IN_CTL_VAL & 0x0000f000) >> 12))
	P1_TX_PRI_WDS=$(($P1_TX_IN_CTL_VAL & 0x000003ff))
	
	P1_PORT_VLAN=$((PORT_BASE + 0x114))
	P1_PORT_VLAN_VAL=`devmem $P1_PORT_VLAN`
	P1_PORT_PRI=$((($P1_PORT_VLAN_VAL & 0x0000e000) >> 13))
	P1_PORT_CFI=$((($P1_PORT_VLAN_VAL & 0x00001000) >> 12))
	P1_PORT_VID=$(($P1_PORT_VLAN_VAL & 0x00000fff))

	P1_TX_PRI_MAP=$((PORT_BASE + 0x118))
	P1_TX_PRI_MAP_VAL=`devmem $P1_TX_PRI_MAP`
	P1_PRI7=$((($P1_TX_PRI_MAP_VAL & 0x30000000) >> 28))
	P1_PRI6=$((($P1_TX_PRI_MAP_VAL & 0x03000000) >> 24))
	P1_PRI5=$((($P1_TX_PRI_MAP_VAL & 0x00300000) >> 20))
	P1_PRI4=$((($P1_TX_PRI_MAP_VAL & 0x00030000) >> 16))
	P1_PRI3=$((($P1_TX_PRI_MAP_VAL & 0x00003000) >> 12))
	P1_PRI2=$((($P1_TX_PRI_MAP_VAL & 0x00000300) >> 8))
	P1_PRI1=$((($P1_TX_PRI_MAP_VAL & 0x00000030) >> 4))
	P1_PRI0=$(($P1_TX_PRI_MAP_VAL & 0x00000003))
	
	P1_TS_SEQ_MTYPE=$((PORT_BASE + 0x11c))
	P1_TS_SEQ_MTYPE_VAL=`devmem $P1_TS_SEQ_MTYPE`
	P1_TS_SEQ_ID_OFFSET=$((($P1_TS_SEQ_MTYPE_VAL & 0x003f0000) >> 16))
	P1_TS_MSG_TYPE_EN=$(($P1_TS_SEQ_MTYPE_VAL & 0x0000ffff))
	
	P1_SA_LO=$((PORT_BASE + 0x120))
	P1_SA_LO_VAL=`devmem $P1_SA_LO`
	P1_MACSRCADDR_7_0=$((($P1_SA_LO_VAL & 0x0000ff00) >> 8))
	P1_MACSRCADDR_15_8=$(($P1_SA_LO_VAL & 0x000000ff))
	
	P1_SA_HI=$((PORT_BASE + 0x124))
	P1_SA_HI_VAL=`devmem $P1_SA_HI`
	P1_MACSRCADDR_23_16=$((($P1_SA_HI_VAL & 0xff000000) >> 24))
	P1_MACSRCADDR_31_24=$((($P1_SA_HI_VAL & 0x00ff0000) >> 16))
	P1_MACSRCADDR_39_32=$((($P1_SA_HI_VAL & 0x0000ff00) >> 8))
	P1_MACSRCADDR_47_40=$(($P1_SA_HI_VAL & 0x000000ff))
	
	P1_SEND_PERCENT=$((PORT_BASE + 0x128))
	P1_SEND_PERCENT_VAL=`devmem $P1_SEND_PERCENT`
	P1_PRI3_SEND_PERCENT=$((($P1_SEND_PERCENT_VAL & 0x007f0000) >> 16))
	P1_PRI2_SEND_PERCENT=$((($P1_SEND_PERCENT_VAL & 0x00007f00) >> 8))
	P1_PRI1_SEND_PERCENT=$(($P1_SEND_PERCENT_VAL  & 0x0000007f))
	
	P1_RX_DSCP_PRI_MAP0=$((PORT_BASE + 0x130))
	P1_RX_DSCP_PRI_MAP0_VAL=`devmem $P1_RX_DSCP_PRI_MAP0`
	P1_RX_PRI7=$((($P1_RX_DSCP_PRI_MAP0_VAL & 0x30000000) >> 28))
	P1_RX_PRI6=$((($P1_RX_DSCP_PRI_MAP0_VAL & 0x03000000) >> 24))
	P1_RX_PRI5=$((($P1_RX_DSCP_PRI_MAP0_VAL & 0x00300000) >> 20))
	P1_RX_PRI4=$((($P1_RX_DSCP_PRI_MAP0_VAL & 0x00030000) >> 16))
	P1_RX_PRI3=$((($P1_RX_DSCP_PRI_MAP0_VAL & 0x00003000) >> 12))
	P1_RX_PRI2=$((($P1_RX_DSCP_PRI_MAP0_VAL & 0x00000300) >> 8))
	P1_RX_PRI1=$((($P1_RX_DSCP_PRI_MAP0_VAL & 0x00000030) >> 4))
	P1_RX_PRI0=$(($P1_RX_DSCP_PRI_MAP0_VAL & 0x00000003))

	P1_RX_DSCP_PRI_MAP1=$((PORT_BASE + 0x134))
	P1_RX_DSCP_PRI_MAP1_VAL=`devmem $P1_RX_DSCP_PRI_MAP1`
	P1_RX_PRI15=$((($P1_RX_DSCP_PRI_MAP1_VAL & 0x30000000) >> 28))
	P1_RX_PRI14=$((($P1_RX_DSCP_PRI_MAP1_VAL & 0x03000000) >> 24))
	P1_RX_PRI13=$((($P1_RX_DSCP_PRI_MAP1_VAL & 0x00300000) >> 20))
	P1_RX_PRI12=$((($P1_RX_DSCP_PRI_MAP1_VAL & 0x00030000) >> 16))
	P1_RX_PRI11=$((($P1_RX_DSCP_PRI_MAP1_VAL & 0x00003000) >> 12))
	P1_RX_PRI10=$((($P1_RX_DSCP_PRI_MAP1_VAL & 0x00000300) >> 8))
	P1_RX_PRI9=$((($P1_RX_DSCP_PRI_MAP1_VAL & 0x00000030) >> 4))
	P1_RX_PRI8=$(($P1_RX_DSCP_PRI_MAP1_VAL & 0x00000003))

	P1_RX_DSCP_PRI_MAP2=$((PORT_BASE + 0x138))
	P1_RX_DSCP_PRI_MAP2_VAL=`devmem $P1_RX_DSCP_PRI_MAP2`
	P1_RX_PRI23=$((($P1_RX_DSCP_PRI_MAP2_VAL & 0x30000000) >> 28))
	P1_RX_PRI22=$((($P1_RX_DSCP_PRI_MAP2_VAL & 0x03000000) >> 24))
	P1_RX_PRI21=$((($P1_RX_DSCP_PRI_MAP2_VAL & 0x00300000) >> 20))
	P1_RX_PRI20=$((($P1_RX_DSCP_PRI_MAP2_VAL & 0x00030000) >> 16))
	P1_RX_PRI19=$((($P1_RX_DSCP_PRI_MAP2_VAL & 0x00003000) >> 12))
	P1_RX_PRI18=$((($P1_RX_DSCP_PRI_MAP2_VAL & 0x00000300) >> 8))
	P1_RX_PRI17=$((($P1_RX_DSCP_PRI_MAP2_VAL & 0x00000030) >> 4))
	P1_RX_PRI16=$(($P1_RX_DSCP_PRI_MAP2_VAL & 0x00000003))
	
	P1_RX_DSCP_PRI_MAP3=$((PORT_BASE + 0x13c))
	P1_RX_DSCP_PRI_MAP3_VAL=`devmem $P1_RX_DSCP_PRI_MAP3`
	P1_RX_PRI31=$((($P1_RX_DSCP_PRI_MAP3_VAL & 0x30000000) >> 28))
	P1_RX_PRI30=$((($P1_RX_DSCP_PRI_MAP3_VAL & 0x03000000) >> 24))
	P1_RX_PRI29=$((($P1_RX_DSCP_PRI_MAP3_VAL & 0x00300000) >> 20))
	P1_RX_PRI28=$((($P1_RX_DSCP_PRI_MAP3_VAL & 0x00030000) >> 16))
	P1_RX_PRI27=$((($P1_RX_DSCP_PRI_MAP3_VAL & 0x00003000) >> 12))
	P1_RX_PRI26=$((($P1_RX_DSCP_PRI_MAP3_VAL & 0x00000300) >> 8))
	P1_RX_PRI25=$((($P1_RX_DSCP_PRI_MAP3_VAL & 0x00000030) >> 4))
	P1_RX_PRI24=$(($P1_RX_DSCP_PRI_MAP3_VAL & 0x00000003))
	
	P1_RX_DSCP_PRI_MAP4=$((PORT_BASE + 0x140))
	P1_RX_DSCP_PRI_MAP4_VAL=`devmem $P1_RX_DSCP_PRI_MAP4`
	P1_RX_PRI39=$((($P1_RX_DSCP_PRI_MAP4_VAL & 0x30000000) >> 28))
	P1_RX_PRI38=$((($P1_RX_DSCP_PRI_MAP4_VAL & 0x03000000) >> 24))
	P1_RX_PRI37=$((($P1_RX_DSCP_PRI_MAP4_VAL & 0x00300000) >> 20))
	P1_RX_PRI36=$((($P1_RX_DSCP_PRI_MAP4_VAL & 0x00030000) >> 16))
	P1_RX_PRI35=$((($P1_RX_DSCP_PRI_MAP4_VAL & 0x00003000) >> 12))
	P1_RX_PRI34=$((($P1_RX_DSCP_PRI_MAP4_VAL & 0x00000300) >> 8))
	P1_RX_PRI33=$((($P1_RX_DSCP_PRI_MAP4_VAL & 0x00000030) >> 4))
	P1_RX_PRI32=$(($P1_RX_DSCP_PRI_MAP4_VAL & 0x00000003))
	
	P1_RX_DSCP_PRI_MAP5=$((PORT_BASE + 0x144))
	P1_RX_DSCP_PRI_MAP5_VAL=`devmem $P1_RX_DSCP_PRI_MAP5`
	P1_RX_PRI47=$((($P1_RX_DSCP_PRI_MAP5_VAL & 0x30000000) >> 28))
	P1_RX_PRI46=$((($P1_RX_DSCP_PRI_MAP5_VAL & 0x03000000) >> 24))
	P1_RX_PRI45=$((($P1_RX_DSCP_PRI_MAP5_VAL & 0x00300000) >> 20))
	P1_RX_PRI44=$((($P1_RX_DSCP_PRI_MAP5_VAL & 0x00030000) >> 16))
	P1_RX_PRI43=$((($P1_RX_DSCP_PRI_MAP5_VAL & 0x00003000) >> 12))
	P1_RX_PRI42=$((($P1_RX_DSCP_PRI_MAP5_VAL & 0x00000300) >> 8))
	P1_RX_PRI41=$((($P1_RX_DSCP_PRI_MAP5_VAL & 0x00000030) >> 4))
	P1_RX_PRI40=$(($P1_RX_DSCP_PRI_MAP5_VAL & 0x00000003))
	
	P1_RX_DSCP_PRI_MAP6=$((PORT_BASE + 0x148))
	P1_RX_DSCP_PRI_MAP6_VAL=`devmem $P1_RX_DSCP_PRI_MAP6`
	P1_RX_PRI55=$((($P1_RX_DSCP_PRI_MAP6_VAL & 0x30000000) >> 28))
	P1_RX_PRI54=$((($P1_RX_DSCP_PRI_MAP6_VAL & 0x03000000) >> 24))
	P1_RX_PRI53=$((($P1_RX_DSCP_PRI_MAP6_VAL & 0x00300000) >> 20))
	P1_RX_PRI52=$((($P1_RX_DSCP_PRI_MAP6_VAL & 0x00030000) >> 16))
	P1_RX_PRI51=$((($P1_RX_DSCP_PRI_MAP6_VAL & 0x00003000) >> 12))
	P1_RX_PRI50=$((($P1_RX_DSCP_PRI_MAP6_VAL & 0x00000300) >> 8))
	P1_RX_PRI49=$((($P1_RX_DSCP_PRI_MAP6_VAL & 0x00000030) >> 4))
	P1_RX_PRI48=$(($P1_RX_DSCP_PRI_MAP6_VAL & 0x00000003))
	
	P1_RX_DSCP_PRI_MAP7=$((PORT_BASE + 0x14c))
	P1_RX_DSCP_PRI_MAP7_VAL=`devmem $P1_RX_DSCP_PRI_MAP7`
	P1_RX_PRI63=$((($P1_RX_DSCP_PRI_MAP7_VAL & 0x30000000) >> 28))
	P1_RX_PRI62=$((($P1_RX_DSCP_PRI_MAP7_VAL & 0x03000000) >> 24))
	P1_RX_PRI61=$((($P1_RX_DSCP_PRI_MAP7_VAL & 0x00300000) >> 20))
	P1_RX_PRI60=$((($P1_RX_DSCP_PRI_MAP7_VAL & 0x00030000) >> 16))
	P1_RX_PRI59=$((($P1_RX_DSCP_PRI_MAP7_VAL & 0x00003000) >> 12))
	P1_RX_PRI58=$((($P1_RX_DSCP_PRI_MAP7_VAL & 0x00000300) >> 8))
	P1_RX_PRI57=$((($P1_RX_DSCP_PRI_MAP7_VAL & 0x00000030) >> 4))
	P1_RX_PRI56=$(($P1_RX_DSCP_PRI_MAP7_VAL & 0x00000003))
	
	P1_IDLE2LPI=$((PORT_BASE + 0x150))
	P1_IDLE2LPI_VAL=`devmem $P1_IDLE2LPI`
	P1_IDLE2LPI_VAL=$(($P1_IDLE2LPI_VAL & 0x000fffff))

	P1_LPI2WAKE=$((PORT_BASE + 0x154))
	P1_LPI2WAKE_VAL=`devmem $P1_LPI2WAKE`
	P1_LPI2WAKE_VAL=$(($P1_LPI2WAKE_VAL & 0x000fffff))
	
	P2_CONTROL=$((PORT_BASE + 0x200))
	P2_CONTROL_VAL=`devmem $P2_CONTROL`
	P2_TX_CLKSTOP_EN=$((($P2_CONTROL_VAL   & 0x02000000) >> 25))
	P2_PASS_PRI_TAGGED=$((($P2_CONTROL_VAL & 0x01000000) >> 24))
	P2_VLAN_LTYPE2_EN=$((($P2_CONTROL_VAL & 0x00200000) >> 21))
	P2_VLAN_LTYPE1_EN=$((($P2_CONTROL_VAL & 0x00100000) >> 20))
	P2_DSCP_PRI_EN=$((($P2_CONTROL_VAL & 0x00010000) >> 16))
	P2_TS_107=$((($P2_CONTROL_VAL & 0x00008000) >> 15))
	P2_TS_320=$((($P2_CONTROL_VAL & 0x00004000) >> 14))
	P2_TS_319=$((($P2_CONTROL_VAL & 0x00002000) >> 13))
	P2_TS_132=$((($P2_CONTROL_VAL & 0x00001000) >> 12))
	P2_TS_131=$((($P2_CONTROL_VAL & 0x00000800) >> 11))
	P2_TS_130=$((($P2_CONTROL_VAL & 0x00000400) >> 10))
	P2_TS_129=$((($P2_CONTROL_VAL & 0x00000200) >> 9))
	P2_TS_TTL_NONZERO=$((($P2_CONTROL_VAL & 0x00000100) >> 8))
	P2_TS_UNI_EN=$((($P2_CONTROL_VAL & 0x00000080) >> 7))
	P2_TS_ANNEX_F_EN=$((($P2_CONTROL_VAL & 0x00000040) >> 6))
	P2_TS_ANNEX_E_EN=$((($P2_CONTROL_VAL & 0x00000020) >> 5))
	P2_TS_ANNEX_D_EN=$((($P2_CONTROL_VAL & 0x00000010) >> 4))
	P2_TS_LTYPE2_EN=$((($P2_CONTROL_VAL & 0x00000008) >> 3))
	P2_TS_LTYPE1_EN=$((($P2_CONTROL_VAL & 0x00000004) >> 2))
	P2_TS_TX_EN=$((($P2_CONTROL_VAL & 0x00000002) >> 1))
	P2_TS_RX_EN=$(($P2_CONTROL_VAL & 0x00000001))

	P2_MAX_BLKS=$((PORT_BASE + 0x208))
	P2_MAX_BLKS_VAL=`devmem $P2_MAX_BLKS`
	P2_TX_MAX_BLKS=$((($P2_MAX_BLKS_VAL & 0x000001f0) >> 4))
	P2_RX_MAX_BLKS=$(($P2_MAX_BLKS_VAL & 0x0000000f))
	
	P2_BLK_CNT=$((PORT_BASE + 0x20c))
	P2_BLK_CNT_VAL=`devmem $P2_BLK_CNT`
	P2_TX_BLK_CNT=$((($P2_BLK_CNT_VAL & 0x000001f0) >> 4))
	P2_RX_MAX_BLKS=$(($P2_BLK_CNT_VAL & 0x0000000f))
	
	P2_TX_IN_CTL=$((PORT_BASE + 0x210))
	P2_TX_IN_CTL_VAL=`devmem $P2_TX_IN_CTL`
	P2_HOST_BLKS_REM=$((($P2_TX_IN_CTL_VAL & 0x0f000000) >> 24))
	P2_TX_RATE_EN=$((($P2_TX_IN_CTL_VAL & 0x00f00000) >> 20))
	P2_TX_IN_SEL=$((($P2_TX_IN_CTL_VAL & 0x00030000) >> 16))
	P2_TX_BLKS_REM=$((($P2_TX_IN_CTL_VAL & 0x0000f000) >> 12))
	P2_TX_PRI_WDS=$(($P2_TX_IN_CTL_VAL & 0x000003ff))
	
	P2_PORT_VLAN=$((PORT_BASE + 0x214))
	P2_PORT_VLAN_VAL=`devmem $P2_PORT_VLAN`
	P2_PORT_PRI=$((($P2_PORT_VLAN_VAL & 0x0000e000) >> 13))
	P2_PORT_CFI=$((($P2_PORT_VLAN_VAL & 0x00001000) >> 12))
	P2_PORT_VID=$(($P2_PORT_VLAN_VAL & 0x00000fff))
	
	P2_TX_PRI_MAP=$((PORT_BASE + 0x218))
	P2_TX_PRI_MAP_VAL=`devmem $P2_TX_PRI_MAP`
	P2_PRI7=$((($P2_TX_PRI_MAP_VAL & 0x30000000) >> 28))
	P2_PRI6=$((($P2_TX_PRI_MAP_VAL & 0x03000000) >> 24))
	P2_PRI5=$((($P2_TX_PRI_MAP_VAL & 0x00300000) >> 20))
	P2_PRI4=$((($P2_TX_PRI_MAP_VAL & 0x00030000) >> 16))
	P2_PRI3=$((($P2_TX_PRI_MAP_VAL & 0x00003000) >> 12))
	P2_PRI2=$((($P2_TX_PRI_MAP_VAL & 0x00000300) >> 8))
	P2_PRI1=$((($P2_TX_PRI_MAP_VAL & 0x00000030) >> 4))
	P2_PRI0=$(($P2_TX_PRI_MAP_VAL & 0x00000003))

	P2_TS_SEQ_MTYPE=$((PORT_BASE + 0x21c))
	P2_TS_SEQ_MTYPE_VAL=`devmem $P2_TS_SEQ_MTYPE`
	P2_TS_SEQ_ID_OFFSET=$((($P2_TS_SEQ_MTYPE_VAL & 0x003f0000) >> 16))
	P2_TS_MSG_TYPE_EN=$(($P2_TS_SEQ_MTYPE_VAL & 0x0000ffff))

	P2_SA_LO=$((PORT_BASE + 0x220))
	P2_SA_LO_VAL=`devmem $P2_SA_LO`
	P2_MACSRCADDR_7_0=$((($P2_SA_LO_VAL & 0x0000ff00) >> 8))
	P2_MACSRCADDR_15_8=$(($P2_SA_LO_VAL & 0x000000ff))
	
	P2_SA_HI=$((PORT_BASE + 0x224))
	P2_SA_HI_VAL=`devmem $P2_SA_HI`
	P2_MACSRCADDR_23_16=$((($P2_SA_HI_VAL & 0xff000000) >> 24))
	P2_MACSRCADDR_31_24=$((($P2_SA_HI_VAL & 0x00ff0000) >> 16))
	P2_MACSRCADDR_39_32=$((($P2_SA_HI_VAL & 0x0000ff00) >> 8))
	P2_MACSRCADDR_47_40=$(($P2_SA_HI_VAL & 0x000000ff))
	
	P2_SEND_PERCENT=$((PORT_BASE + 0x228))
	P2_SEND_PERCENT_VAL=`devmem $P2_SEND_PERCENT`
	P2_PRI3_SEND_PERCENT=$((($P2_SEND_PERCENT_VAL & 0x007f0000) >> 16))
	P2_PRI2_SEND_PERCENT=$((($P2_SEND_PERCENT_VAL & 0x00007f00) >> 8))
	P2_PRI1_SEND_PERCENT=$(($P2_SEND_PERCENT_VAL  & 0x0000007f))
	
	P2_RX_DSCP_PRI_MAP0=$((PORT_BASE + 0x230))
	P2_RX_DSCP_PRI_MAP0_VAL=`devmem $P2_RX_DSCP_PRI_MAP0`
	P2_RX_PRI7=$((($P2_RX_DSCP_PRI_MAP0_VAL & 0x30000000) >> 28))
	P2_RX_PRI6=$((($P2_RX_DSCP_PRI_MAP0_VAL & 0x03000000) >> 24))
	P2_RX_PRI5=$((($P2_RX_DSCP_PRI_MAP0_VAL & 0x00300000) >> 20))
	P2_RX_PRI4=$((($P2_RX_DSCP_PRI_MAP0_VAL & 0x00030000) >> 16))
	P2_RX_PRI3=$((($P2_RX_DSCP_PRI_MAP0_VAL & 0x00003000) >> 12))
	P2_RX_PRI2=$((($P2_RX_DSCP_PRI_MAP0_VAL & 0x00000300) >> 8))
	P2_RX_PRI1=$((($P2_RX_DSCP_PRI_MAP0_VAL & 0x00000030) >> 4))
	P2_RX_PRI0=$(($P2_RX_DSCP_PRI_MAP0_VAL & 0x00000003))

	P2_RX_DSCP_PRI_MAP1=$((PORT_BASE + 0x234))
	P2_RX_DSCP_PRI_MAP1_VAL=`devmem $P2_RX_DSCP_PRI_MAP1`
	P2_RX_PRI15=$((($P2_RX_DSCP_PRI_MAP1_VAL & 0x30000000) >> 28))
	P2_RX_PRI14=$((($P2_RX_DSCP_PRI_MAP1_VAL & 0x03000000) >> 24))
	P2_RX_PRI13=$((($P2_RX_DSCP_PRI_MAP1_VAL & 0x00300000) >> 20))
	P2_RX_PRI12=$((($P2_RX_DSCP_PRI_MAP1_VAL & 0x00030000) >> 16))
	P2_RX_PRI11=$((($P2_RX_DSCP_PRI_MAP1_VAL & 0x00003000) >> 12))
	P2_RX_PRI10=$((($P2_RX_DSCP_PRI_MAP1_VAL & 0x00000300) >> 8))
	P2_RX_PRI9=$((($P2_RX_DSCP_PRI_MAP1_VAL & 0x00000030) >> 4))
	P2_RX_PRI8=$(($P2_RX_DSCP_PRI_MAP1_VAL & 0x00000003))

	P2_RX_DSCP_PRI_MAP2=$((PORT_BASE + 0x238))
	P2_RX_DSCP_PRI_MAP2_VAL=`devmem $P2_RX_DSCP_PRI_MAP2`
	P2_RX_PRI23=$((($P2_RX_DSCP_PRI_MAP2_VAL & 0x30000000) >> 28))
	P2_RX_PRI22=$((($P2_RX_DSCP_PRI_MAP2_VAL & 0x03000000) >> 24))
	P2_RX_PRI21=$((($P2_RX_DSCP_PRI_MAP2_VAL & 0x00300000) >> 20))
	P2_RX_PRI20=$((($P2_RX_DSCP_PRI_MAP2_VAL & 0x00030000) >> 16))
	P2_RX_PRI19=$((($P2_RX_DSCP_PRI_MAP2_VAL & 0x00003000) >> 12))
	P2_RX_PRI18=$((($P2_RX_DSCP_PRI_MAP2_VAL & 0x00000300) >> 8))
	P2_RX_PRI17=$((($P2_RX_DSCP_PRI_MAP2_VAL & 0x00000030) >> 4))
	P2_RX_PRI16=$(($P2_RX_DSCP_PRI_MAP2_VAL & 0x00000003))
	
	P2_RX_DSCP_PRI_MAP3=$((PORT_BASE + 0x23c))
	P2_RX_DSCP_PRI_MAP3_VAL=`devmem $P2_RX_DSCP_PRI_MAP3`
	P2_RX_PRI31=$((($P2_RX_DSCP_PRI_MAP3_VAL & 0x30000000) >> 28))
	P2_RX_PRI30=$((($P2_RX_DSCP_PRI_MAP3_VAL & 0x03000000) >> 24))
	P2_RX_PRI29=$((($P2_RX_DSCP_PRI_MAP3_VAL & 0x00300000) >> 20))
	P2_RX_PRI28=$((($P2_RX_DSCP_PRI_MAP3_VAL & 0x00030000) >> 16))
	P2_RX_PRI27=$((($P2_RX_DSCP_PRI_MAP3_VAL & 0x00003000) >> 12))
	P2_RX_PRI26=$((($P2_RX_DSCP_PRI_MAP3_VAL & 0x00000300) >> 8))
	P2_RX_PRI25=$((($P2_RX_DSCP_PRI_MAP3_VAL & 0x00000030) >> 4))
	P2_RX_PRI24=$(($P2_RX_DSCP_PRI_MAP3_VAL & 0x00000003))
	
	P2_RX_DSCP_PRI_MAP4=$((PORT_BASE + 0x240))
	P2_RX_DSCP_PRI_MAP4_VAL=`devmem $P2_RX_DSCP_PRI_MAP4`
	P2_RX_PRI39=$((($P2_RX_DSCP_PRI_MAP4_VAL & 0x30000000) >> 28))
	P2_RX_PRI38=$((($P2_RX_DSCP_PRI_MAP4_VAL & 0x03000000) >> 24))
	P2_RX_PRI37=$((($P2_RX_DSCP_PRI_MAP4_VAL & 0x00300000) >> 20))
	P2_RX_PRI36=$((($P2_RX_DSCP_PRI_MAP4_VAL & 0x00030000) >> 16))
	P2_RX_PRI35=$((($P2_RX_DSCP_PRI_MAP4_VAL & 0x00003000) >> 12))
	P2_RX_PRI34=$((($P2_RX_DSCP_PRI_MAP4_VAL & 0x00000300) >> 8))
	P2_RX_PRI33=$((($P2_RX_DSCP_PRI_MAP4_VAL & 0x00000030) >> 4))
	P2_RX_PRI32=$(($P2_RX_DSCP_PRI_MAP4_VAL & 0x00000003))
	
	P2_RX_DSCP_PRI_MAP5=$((PORT_BASE + 0x244))
	P2_RX_DSCP_PRI_MAP5_VAL=`devmem $P2_RX_DSCP_PRI_MAP5`
	P2_RX_PRI47=$((($P2_RX_DSCP_PRI_MAP5_VAL & 0x30000000) >> 28))
	P2_RX_PRI46=$((($P2_RX_DSCP_PRI_MAP5_VAL & 0x03000000) >> 24))
	P2_RX_PRI45=$((($P2_RX_DSCP_PRI_MAP5_VAL & 0x00300000) >> 20))
	P2_RX_PRI44=$((($P2_RX_DSCP_PRI_MAP5_VAL & 0x00030000) >> 16))
	P2_RX_PRI43=$((($P2_RX_DSCP_PRI_MAP5_VAL & 0x00003000) >> 12))
	P2_RX_PRI42=$((($P2_RX_DSCP_PRI_MAP5_VAL & 0x00000300) >> 8))
	P2_RX_PRI41=$((($P2_RX_DSCP_PRI_MAP5_VAL & 0x00000030) >> 4))
	P2_RX_PRI40=$(($P2_RX_DSCP_PRI_MAP5_VAL & 0x00000003))
	
	P2_RX_DSCP_PRI_MAP6=$((PORT_BASE + 0x248))
	P2_RX_DSCP_PRI_MAP6_VAL=`devmem $P2_RX_DSCP_PRI_MAP6`
	P2_RX_PRI55=$((($P2_RX_DSCP_PRI_MAP6_VAL & 0x30000000) >> 28))
	P2_RX_PRI54=$((($P2_RX_DSCP_PRI_MAP6_VAL & 0x03000000) >> 24))
	P2_RX_PRI53=$((($P2_RX_DSCP_PRI_MAP6_VAL & 0x00300000) >> 20))
	P2_RX_PRI52=$((($P2_RX_DSCP_PRI_MAP6_VAL & 0x00030000) >> 16))
	P2_RX_PRI51=$((($P2_RX_DSCP_PRI_MAP6_VAL & 0x00003000) >> 12))
	P2_RX_PRI50=$((($P2_RX_DSCP_PRI_MAP6_VAL & 0x00000300) >> 8))
	P2_RX_PRI49=$((($P2_RX_DSCP_PRI_MAP6_VAL & 0x00000030) >> 4))
	P2_RX_PRI48=$(($P2_RX_DSCP_PRI_MAP6_VAL & 0x00000003))
	
	P2_RX_DSCP_PRI_MAP7=$((PORT_BASE + 0x24c))
	P2_RX_DSCP_PRI_MAP7_VAL=`devmem $P2_RX_DSCP_PRI_MAP7`
	P2_RX_PRI63=$((($P2_RX_DSCP_PRI_MAP7_VAL & 0x30000000) >> 28))
	P2_RX_PRI62=$((($P2_RX_DSCP_PRI_MAP7_VAL & 0x03000000) >> 24))
	P2_RX_PRI61=$((($P2_RX_DSCP_PRI_MAP7_VAL & 0x00300000) >> 20))
	P2_RX_PRI60=$((($P2_RX_DSCP_PRI_MAP7_VAL & 0x00030000) >> 16))
	P2_RX_PRI59=$((($P2_RX_DSCP_PRI_MAP7_VAL & 0x00003000) >> 12))
	P2_RX_PRI58=$((($P2_RX_DSCP_PRI_MAP7_VAL & 0x00000300) >> 8))
	P2_RX_PRI57=$((($P2_RX_DSCP_PRI_MAP7_VAL & 0x00000030) >> 4))
	P2_RX_PRI56=$(($P2_RX_DSCP_PRI_MAP7_VAL & 0x00000003))

	P2_IDLE2LPI=$((PORT_BASE + 0x250))
	P2_IDLE2LPI_VAL=`devmem $P2_IDLE2LPI`
	P2_IDLE2LPI_VAL=$(($P2_IDLE2LPI_VAL & 0x000fffff))
	
	P2_LPI2WAKE=$((PORT_BASE + 0x2540))
	P2_LPI2WAKE_VAL=`devmem $P2_LPI2WAKE`
	P2_LPI2WAKE_VAL=$(($P2_LPI2WAKE_VAL & 0x000fffff))

	printf "==================PORT================================-\n"

	printf "        P0_CONTROL\n"
	printf "              P0_DLR_CPDMA_CH     %u\n" $P0_DLR_CPDMA_CH
	printf "              P0_PASS_PRI_TAGGED  %u\n" $P0_PASS_PRI_TAGGED
	printf "              P0_VLAN_LTYPE2_EN   %u\n" $P0_VLAN_LTYPE2_EN
	printf "              P0_VLAN_LTYPE1_EN   %u\n" $P0_VLAN_LTYPE1_EN
	printf "        P0_MAX_BLKS\n"
	printf "              P0_TX_MAX_BLKS      %u\n" $P0_TX_MAX_BLKS
	printf "              P0_RX_MAX_BLKS      %u\n" $P0_RX_MAX_BLKS
	printf "        P0_BLK_CNT\n"
	printf "              P0_TX_BLK_CNT       %u\n" $P0_TX_BLK_CNT
	printf "              P0_RX_BLK_CNT       %u\n" $P0_RX_BLK_CNT
	printf "        P0_TX_IN_CTL\n"
	printf "              TX_RATE_EN          %u\n" $P0_TX_RATE_EN
	printf "              TX_IN_SEL           %u\n" $P0_TX_IN_SEL
	printf "              TX_BLKS_REM         %u\n" $P0_TX_BLKS_REM
	printf "              TX_PRI_WDS          %u\n" $P0_TX_PRI_WDS
	printf "        P0_PORT_VLAN\n"
	printf "              PORT_PRI            %u\n" $P0_PORT_PRI
	printf "              PORT_CFI            %u\n" $P0_PORT_CFI
	printf "              PORT_VID            %u\n" $P0_PORT_VID
	printf "        P0_TX_PRI_MAP\n"
	printf "              PRI7                %u\n" $P0_PRI7
	printf "              PRI6                %u\n" $P0_PRI6
	printf "              PRI5                %u\n" $P0_PRI5
	printf "              PRI4                %u\n" $P0_PRI4
	printf "              PRI3                %u\n" $P0_PRI3
	printf "              PRI2                %u\n" $P0_PRI2
	printf "              PRI1                %u\n" $P0_PRI1
	printf "              PRI0                %u\n" $P0_PRI0
	printf "        P0_CPDMA_TX_PRI_MAP\n"
	printf "              PRI7                %u\n" $P0_CPDMA_TX_PRI7
	printf "              PRI6                %u\n" $P0_CPDMA_TX_PRI6
	printf "              PRI5                %u\n" $P0_CPDMA_TX_PRI5
	printf "              PRI4                %u\n" $P0_CPDMA_TX_PRI4
	printf "              PRI3                %u\n" $P0_CPDMA_TX_PRI3
	printf "              PRI2                %u\n" $P0_CPDMA_TX_PRI2
	printf "              PRI1                %u\n" $P0_CPDMA_TX_PRI1
	printf "              PRI0                %u\n" $P0_CPDMA_TX_PRI0
	printf "        P0_CPDMA_RX_CH_MAP\n"
	printf "              P2_PRI3             %u\n" $P2_PRI3
	printf "              P2_PRI2             %u\n" $P2_PRI2
	printf "              P2_PRI1             %u\n" $P2_PRI1
	printf "              P2_PRI0             %u\n" $P2_PRI0
	printf "              P1_PRI3             %u\n" $P1_PRI3
	printf "              P1_PRI2             %u\n" $P1_PRI2
	printf "              P1_PRI1             %u\n" $P1_PRI1
	printf "              P1_PRI0             %u\n" $P1_PRI0
	printf "        P0_RX_DSCP_PRI_MAP0\n"
	printf "              PRI7                %u\n" $P0_RX_PRI7
	printf "              PRI6                %u\n" $P0_RX_PRI6
	printf "              PRI5                %u\n" $P0_RX_PRI5
	printf "              PRI4                %u\n" $P0_RX_PRI4
	printf "              PRI3                %u\n" $P0_RX_PRI3
	printf "              PRI2                %u\n" $P0_RX_PRI2
	printf "              PRI1                %u\n" $P0_RX_PRI1
	printf "              PRI0                %u\n" $P0_RX_PRI0
	printf "        P0_RX_DSCP_PRI_MAP1\n"
	printf "              PRI15               %u\n" $P0_RX_PRI15
	printf "              PRI14               %u\n" $P0_RX_PRI14
	printf "              PRI13               %u\n" $P0_RX_PRI13
	printf "              PRI12               %u\n" $P0_RX_PRI12
	printf "              PRI11               %u\n" $P0_RX_PRI11
	printf "              PRI10               %u\n" $P0_RX_PRI10
	printf "              PRI9                %u\n" $P0_RX_PRI9
	printf "              PRI8                %u\n" $P0_RX_PRI8
	printf "        P0_RX_DSCP_PRI_MAP2\n"
	printf "              PRI23               %u\n" $P0_RX_PRI23
	printf "              PRI22               %u\n" $P0_RX_PRI22
	printf "              PRI21               %u\n" $P0_RX_PRI21
	printf "              PRI20               %u\n" $P0_RX_PRI20
	printf "              PRI19               %u\n" $P0_RX_PRI19
	printf "              PRI18               %u\n" $P0_RX_PRI18
	printf "              PRI17               %u\n" $P0_RX_PRI17
	printf "              PRI16               %u\n" $P0_RX_PRI16
	printf "        P0_RX_DSCP_PRI_MAP3\n"
	printf "              PRI31               %u\n" $P0_RX_PRI31
	printf "              PRI30               %u\n" $P0_RX_PRI30
	printf "              PRI29               %u\n" $P0_RX_PRI29
	printf "              PRI28               %u\n" $P0_RX_PRI28
	printf "              PRI27               %u\n" $P0_RX_PRI27
	printf "              PRI26               %u\n" $P0_RX_PRI26
	printf "              PRI25               %u\n" $P0_RX_PRI25
	printf "              PRI24               %u\n" $P0_RX_PRI24
	printf "        P0_RX_DSCP_PRI_MAP4\n"
	printf "              PRI39               %u\n" $P0_RX_PRI39
	printf "              PRI38               %u\n" $P0_RX_PRI38
	printf "              PRI37               %u\n" $P0_RX_PRI37
	printf "              PRI36               %u\n" $P0_RX_PRI36
	printf "              PRI35               %u\n" $P0_RX_PRI35
	printf "              PRI34               %u\n" $P0_RX_PRI34
	printf "              PRI33               %u\n" $P0_RX_PRI33
	printf "              PRI32               %u\n" $P0_RX_PRI32
	printf "        P0_RX_DSCP_PRI_MAP5\n"
	printf "              PRI47               %u\n" $P0_RX_PRI47
	printf "              PRI46               %u\n" $P0_RX_PRI46
	printf "              PRI45               %u\n" $P0_RX_PRI45
	printf "              PRI44               %u\n" $P0_RX_PRI44
	printf "              PRI43               %u\n" $P0_RX_PRI43
	printf "              PRI42               %u\n" $P0_RX_PRI42
	printf "              PRI41               %u\n" $P0_RX_PRI41
	printf "              PRI40               %u\n" $P0_RX_PRI40
	printf "        P0_RX_DSCP_PRI_MAP6\n"
	printf "              PRI55               %u\n" $P0_RX_PRI55
	printf "              PRI54               %u\n" $P0_RX_PRI54
	printf "              PRI53               %u\n" $P0_RX_PRI53
	printf "              PRI52               %u\n" $P0_RX_PRI52
	printf "              PRI51               %u\n" $P0_RX_PRI51
	printf "              PRI50               %u\n" $P0_RX_PRI50
	printf "              PRI49               %u\n" $P0_RX_PRI49
	printf "              PRI48               %u\n" $P0_RX_PRI48
	printf "        P0_RX_DSCP_PRI_MAP7\n"
	printf "              PRI63               %u\n" $P0_RX_PRI63
	printf "              PRI62               %u\n" $P0_RX_PRI62
	printf "              PRI61               %u\n" $P0_RX_PRI61
	printf "              PRI60               %u\n" $P0_RX_PRI60
	printf "              PRI59               %u\n" $P0_RX_PRI59
	printf "              PRI58               %u\n" $P0_RX_PRI58
	printf "              PRI57               %u\n" $P0_RX_PRI57
	printf "              PRI56               %u\n" $P0_RX_PRI56
	printf "        P0_IDLE2LPI\n"
	printf "              P0_IDLE2LPI         %u\n" $P0_IDLE2LPI_VAL
	printf "        P0_LPI2WAKE\n"
	printf "              P0_LPI2WAKE         %u\n" $P0_LPI2WAKE_VAL
	printf "        P1_CONTROL\n"
	printf "              P1_TX_CLKSTOP_EN    %u\n" $P1_TX_CLKSTOP_EN
	printf "              P1_PASS_PRI_TAGGED  %u\n" $P1_PASS_PRI_TAGGED
	printf "              P1_VLAN_LTYPE2_EN   %u\n" $P1_VLAN_LTYPE2_EN
	printf "              P1_VLAN_LTYPE1_EN   %u\n" $P1_VLAN_LTYPE1_EN
	printf "              P1_DSCP_PRI_EN      %u\n" $P1_DSCP_PRI_EN
	printf "              P1_TS_107           %u\n" $P1_TS_107
	printf "              P1_TS_320           %u\n" $P1_TS_320
	printf "              P1_TS_319           %u\n" $P1_TS_319
	printf "              P1_TS_132           %u\n" $P1_TS_132
	printf "              P1_TS_131           %u\n" $P1_TS_131
	printf "              P1_TS_130           %u\n" $P1_TS_130
	printf "              P1_TS_129           %u\n" $P1_TS_129
	printf "              P1_TS_TTL_NONZERO   %u\n" $P1_TS_TTL_NONZERO
	printf "              P1_TS_UNI_EN        %u\n" $P1_TS_UNI_EN
	printf "              P1_TS_ANNEX_F_EN    %u\n" $P1_TS_ANNEX_F_EN
	printf "              P1_TS_ANNEX_E_EN    %u\n" $P1_TS_ANNEX_E_EN
	printf "              P1_TS_ANNEX_D_EN    %u\n" $P1_TS_ANNEX_D_EN
	printf "              P1_TS_LTYPE2_EN     %u\n" $P1_TS_LTYPE2_EN
	printf "              P1_TS_LTYPE1_EN     %u\n" $P1_TS_LTYPE1_EN
	printf "              P1_TS_TX_EN         %u\n" $P1_TS_TX_EN
	printf "              P1_TS_RX_EN         %u\n" $P1_TS_RX_EN
	printf "        P1_MAX_BLKS\n"
	printf "              P1_TX_MAX_BLKS      %u\n" $P1_TX_MAX_BLKS
	printf "              P1_RX_MAX_BLKS      %u\n" $P1_RX_MAX_BLKS
	printf "        P1_BLK_CNT\n"
	printf "              P1_TX_BLK_CNT       %u\n" $P1_TX_BLK_CNT
	printf "              P1_RX_BLK_CNT       %u\n" $P1_RX_BLK_CNT
	printf "        P1_TX_IN_CTL\n"
	printf "              HOST_BLKS_REM       %u\n" $P1_HOST_BLKS_REM
	printf "              TX_RATE_EN          %u\n" $P1_TX_RATE_EN
	printf "              TX_IN_SEL           %u\n" $P1_TX_IN_SEL
	printf "              TX_BLKS_REM         %u\n" $P1_TX_BLKS_REM
	printf "              TX_PRI_WDS          %u\n" $P1_TX_PRI_WDS
	printf "        P1_PORT_VLAN\n"
	printf "              PORT_PRI            %u\n" $P1_PORT_PRI
	printf "              PORT_CFI            %u\n" $P1_PORT_CFI
	printf "              PORT_VID            %u\n" $P1_PORT_VID
	printf "        P1_TX_PRI_MAP\n"
	printf "              PRI7                %u\n" $P1_PRI7
	printf "              PRI6                %u\n" $P1_PRI6
	printf "              PRI5                %u\n" $P1_PRI5
	printf "              PRI4                %u\n" $P1_PRI4
	printf "              PRI3                %u\n" $P1_PRI3
	printf "              PRI2                %u\n" $P1_PRI2
	printf "              PRI1                %u\n" $P1_PRI1
	printf "              PRI0                %u\n" $P1_PRI0
	printf "        P1_TS_SEQ_MTYPE\n"
	printf "              P1_TS_SEQ_ID_OFFSET %u\n" $P1_TS_SEQ_ID_OFFSET
	printf "              P1_TS_MSG_TYPE_EN   %u\n" $P1_TS_MSG_TYPE_EN
	printf "        P1_SA_LO\n"
	printf "              MACSRCADDR_7_0      0x%02x\n" $P1_MACSRCADDR_7_0
	printf "              MACSRCADDR_15_8     0x%02x\n" $P1_MACSRCADDR_15_8
	printf "        P1_SA_HI\n"
	printf "              MACSRCADDR_23_16    0x%02x\n" $P1_MACSRCADDR_23_16
	printf "              MACSRCADDR_31_24    0x%02x\n" $P1_MACSRCADDR_31_24
	printf "              MACSRCADDR_39_32    0x%02x\n" $P1_MACSRCADDR_39_32
	printf "              MACSRCADDR_47_40    0x%02x\n" $P1_MACSRCADDR_47_40
	printf "        P1_SEND_PERCENT\n"
	printf "              PRI3_SEND_PERCENT   %u\n" $P1_PRI3_SEND_PERCENT
	printf "              PRI2_SEND_PERCENT   %u\n" $P1_PRI2_SEND_PERCENT
	printf "              PRI1_SEND_PERCENT   %u\n" $P1_PRI1_SEND_PERCENT
	printf "        P1_RX_DSCP_PRI_MAP0\n"
	printf "              PRI7                %u\n" $P1_RX_PRI7
	printf "              PRI6                %u\n" $P1_RX_PRI6
	printf "              PRI5                %u\n" $P1_RX_PRI5
	printf "              PRI4                %u\n" $P1_RX_PRI4
	printf "              PRI3                %u\n" $P1_RX_PRI3
	printf "              PRI2                %u\n" $P1_RX_PRI2
	printf "              PRI1                %u\n" $P1_RX_PRI1
	printf "              PRI0                %u\n" $P1_RX_PRI0
	printf "        P1_RX_DSCP_PRI_MAP1\n"
	printf "              PRI15               %u\n" $P1_RX_PRI15
	printf "              PRI14               %u\n" $P1_RX_PRI14
	printf "              PRI13               %u\n" $P1_RX_PRI13
	printf "              PRI12               %u\n" $P1_RX_PRI12
	printf "              PRI11               %u\n" $P1_RX_PRI11
	printf "              PRI10               %u\n" $P1_RX_PRI10
	printf "              PRI9                %u\n" $P1_RX_PRI9
	printf "              PRI8                %u\n" $P1_RX_PRI8
	printf "        P1_RX_DSCP_PRI_MAP2\n"
	printf "              PRI23               %u\n" $P1_RX_PRI23
	printf "              PRI22               %u\n" $P1_RX_PRI22
	printf "              PRI21               %u\n" $P1_RX_PRI21
	printf "              PRI20               %u\n" $P1_RX_PRI20
	printf "              PRI19               %u\n" $P1_RX_PRI19
	printf "              PRI18               %u\n" $P1_RX_PRI18
	printf "              PRI17               %u\n" $P1_RX_PRI17
	printf "              PRI16               %u\n" $P1_RX_PRI16
	printf "        P1_RX_DSCP_PRI_MAP3\n"
	printf "              PRI31               %u\n" $P1_RX_PRI31
	printf "              PRI30               %u\n" $P1_RX_PRI30
	printf "              PRI29               %u\n" $P1_RX_PRI29
	printf "              PRI28               %u\n" $P1_RX_PRI28
	printf "              PRI27               %u\n" $P1_RX_PRI27
	printf "              PRI26               %u\n" $P1_RX_PRI26
	printf "              PRI25               %u\n" $P1_RX_PRI25
	printf "              PRI24               %u\n" $P1_RX_PRI24
	printf "        P1_RX_DSCP_PRI_MAP4\n"
	printf "              PRI39               %u\n" $P1_RX_PRI39
	printf "              PRI38               %u\n" $P1_RX_PRI38
	printf "              PRI37               %u\n" $P1_RX_PRI37
	printf "              PRI36               %u\n" $P1_RX_PRI36
	printf "              PRI35               %u\n" $P1_RX_PRI35
	printf "              PRI34               %u\n" $P1_RX_PRI34
	printf "              PRI33               %u\n" $P1_RX_PRI33
	printf "              PRI32               %u\n" $P1_RX_PRI32
	printf "        P1_RX_DSCP_PRI_MAP5\n"
	printf "              PRI47               %u\n" $P1_RX_PRI47
	printf "              PRI46               %u\n" $P1_RX_PRI46
	printf "              PRI45               %u\n" $P1_RX_PRI45
	printf "              PRI44               %u\n" $P1_RX_PRI44
	printf "              PRI43               %u\n" $P1_RX_PRI43
	printf "              PRI42               %u\n" $P1_RX_PRI42
	printf "              PRI41               %u\n" $P1_RX_PRI41
	printf "              PRI40               %u\n" $P1_RX_PRI40
	printf "        P1_RX_DSCP_PRI_MAP6\n"
	printf "              PRI55               %u\n" $P1_RX_PRI55
	printf "              PRI54               %u\n" $P1_RX_PRI54
	printf "              PRI53               %u\n" $P1_RX_PRI53
	printf "              PRI52               %u\n" $P1_RX_PRI52
	printf "              PRI51               %u\n" $P1_RX_PRI51
	printf "              PRI50               %u\n" $P1_RX_PRI50
	printf "              PRI49               %u\n" $P1_RX_PRI49
	printf "              PRI48               %u\n" $P1_RX_PRI48
	printf "        P1_RX_DSCP_PRI_MAP7\n"
	printf "              PRI63               %u\n" $P1_RX_PRI63
	printf "              PRI62               %u\n" $P1_RX_PRI62
	printf "              PRI61               %u\n" $P1_RX_PRI61
	printf "              PRI60               %u\n" $P1_RX_PRI60
	printf "              PRI59               %u\n" $P1_RX_PRI59
	printf "              PRI58               %u\n" $P1_RX_PRI58
	printf "              PRI57               %u\n" $P1_RX_PRI57
	printf "              PRI56               %u\n" $P1_RX_PRI56
	printf "        P1_IDLE2LPI\n"
	printf "              P1_IDLE2LPI         %u\n" $P1_IDLE2LPI_VAL
	printf "        P1_LPI2WAKE\n"
	printf "              P1_LPI2WAKE         %u\n" $P1_LPI2WAKE_VAL
	printf "        P2_CONTROL\n"
	printf "              P2_TX_CLKSTOP_EN    %u\n" $P2_TX_CLKSTOP_EN
	printf "              P2_PASS_PRI_TAGGED  %u\n" $P2_PASS_PRI_TAGGED
	printf "              P2_VLAN_LTYPE2_EN   %u\n" $P2_VLAN_LTYPE2_EN
	printf "              P2_VLAN_LTYPE1_EN   %u\n" $P2_VLAN_LTYPE1_EN
	printf "              P2_DSCP_PRI_EN      %u\n" $P2_DSCP_PRI_EN
	printf "              P2_TS_107           %u\n" $P2_TS_107
	printf "              P2_TS_320           %u\n" $P2_TS_320
	printf "              P2_TS_319           %u\n" $P2_TS_319
	printf "              P2_TS_132           %u\n" $P2_TS_132
	printf "              P2_TS_131           %u\n" $P2_TS_131
	printf "              P2_TS_130           %u\n" $P2_TS_130
	printf "              P2_TS_129           %u\n" $P2_TS_129
	printf "              P2_TS_TTL_NONZERO   %u\n" $P2_TS_TTL_NONZERO
	printf "              P2_TS_UNI_EN        %u\n" $P2_TS_UNI_EN
	printf "              P2_TS_ANNEX_F_EN    %u\n" $P2_TS_ANNEX_F_EN
	printf "              P2_TS_ANNEX_E_EN    %u\n" $P2_TS_ANNEX_E_EN
	printf "              P2_TS_ANNEX_D_EN    %u\n" $P2_TS_ANNEX_D_EN
	printf "              P2_TS_LTYPE2_EN     %u\n" $P2_TS_LTYPE2_EN
	printf "              P2_TS_LTYPE1_EN     %u\n" $P2_TS_LTYPE1_EN
	printf "              P2_TS_TX_EN         %u\n" $P2_TS_TX_EN
	printf "              P2_TS_RX_EN         %u\n" $P2_TS_RX_EN
	printf "        P2_MAX_BLKS\n"
	printf "              P2_TX_MAX_BLKS      %u\n" $P2_TX_MAX_BLKS
	printf "              P2_RX_MAX_BLKS      %u\n" $P2_RX_MAX_BLKS
	printf "        P2_BLK_CNT\n"
	printf "              P2_TX_BLK_CNT       %u\n" $P2_TX_BLK_CNT
	printf "              P2_RX_BLK_CNT       %u\n" $P2_RX_BLK_CNT
	printf "        P2_TX_IN_CTL\n"
	printf "              HOST_BLKS_REM       %u\n" $P2_HOST_BLKS_REM
	printf "              TX_RATE_EN          %u\n" $P2_TX_RATE_EN
	printf "              TX_IN_SEL           %u\n" $P2_TX_IN_SEL
	printf "              TX_BLKS_REM         %u\n" $P2_TX_BLKS_REM
	printf "              TX_PRI_WDS          %u\n" $P2_TX_PRI_WDS
	printf "        P2_PORT_VLAN\n"
	printf "              PORT_PRI            %u\n" $P2_PORT_PRI
	printf "              PORT_CFI            %u\n" $P2_PORT_CFI
	printf "              PORT_VID            %u\n" $P2_PORT_VID
	printf "        P2_TX_PRI_MAP\n"
	printf "              PRI7                %u\n" $P2_PRI7
	printf "              PRI6                %u\n" $P2_PRI6
	printf "              PRI5                %u\n" $P2_PRI5
	printf "              PRI4                %u\n" $P2_PRI4
	printf "              PRI3                %u\n" $P2_PRI3
	printf "              PRI2                %u\n" $P2_PRI2
	printf "              PRI1                %u\n" $P2_PRI1
	printf "              PRI0                %u\n" $P2_PRI0
	printf "        P2_TS_SEQ_MTYPE\n"
	printf "              P2_TS_SEQ_ID_OFFSET %u\n" $P2_TS_SEQ_ID_OFFSET
	printf "              P2_TS_MSG_TYPE_EN   %u\n" $P2_TS_MSG_TYPE_EN
	printf "        P2_SA_LO\n"
	printf "              MACSRCADDR_7_0      0x%02x\n" $P2_MACSRCADDR_7_0
	printf "              MACSRCADDR_15_8     0x%02x\n" $P2_MACSRCADDR_15_8
	printf "        P2_SA_HI\n"
	printf "              MACSRCADDR_23_16    0x%02x\n" $P2_MACSRCADDR_23_16
	printf "              MACSRCADDR_31_24    0x%02x\n" $P2_MACSRCADDR_31_24
	printf "              MACSRCADDR_39_32    0x%02x\n" $P2_MACSRCADDR_39_32
	printf "              MACSRCADDR_47_40    0x%02x\n" $P2_MACSRCADDR_47_40
	printf "        P2_SEND_PERCENT\n"
	printf "              PRI3_SEND_PERCENT   %u\n" $P2_PRI3_SEND_PERCENT
	printf "              PRI2_SEND_PERCENT   %u\n" $P2_PRI2_SEND_PERCENT
	printf "              PRI1_SEND_PERCENT   %u\n" $P2_PRI1_SEND_PERCENT
	printf "        P2_RX_DSCP_PRI_MAP0\n"
	printf "              PRI7                %u\n" $P2_RX_PRI7
	printf "              PRI6                %u\n" $P2_RX_PRI6
	printf "              PRI5                %u\n" $P2_RX_PRI5
	printf "              PRI4                %u\n" $P2_RX_PRI4
	printf "              PRI3                %u\n" $P2_RX_PRI3
	printf "              PRI2                %u\n" $P2_RX_PRI2
	printf "              PRI1                %u\n" $P2_RX_PRI1
	printf "              PRI0                %u\n" $P2_RX_PRI0
	printf "        P2_RX_DSCP_PRI_MAP1\n"
	printf "              PRI15               %u\n" $P2_RX_PRI15
	printf "              PRI14               %u\n" $P2_RX_PRI14
	printf "              PRI13               %u\n" $P2_RX_PRI13
	printf "              PRI12               %u\n" $P2_RX_PRI12
	printf "              PRI11               %u\n" $P2_RX_PRI11
	printf "              PRI10               %u\n" $P2_RX_PRI10
	printf "              PRI9                %u\n" $P2_RX_PRI9
	printf "              PRI8                %u\n" $P2_RX_PRI8
	printf "        P2_RX_DSCP_PRI_MAP2\n"
	printf "              PRI23               %u\n" $P2_RX_PRI23
	printf "              PRI22               %u\n" $P2_RX_PRI22
	printf "              PRI21               %u\n" $P2_RX_PRI21
	printf "              PRI20               %u\n" $P2_RX_PRI20
	printf "              PRI19               %u\n" $P2_RX_PRI19
	printf "              PRI18               %u\n" $P2_RX_PRI18
	printf "              PRI17               %u\n" $P2_RX_PRI17
	printf "              PRI16               %u\n" $P2_RX_PRI16
	printf "        P2_RX_DSCP_PRI_MAP3\n"
	printf "              PRI31               %u\n" $P2_RX_PRI31
	printf "              PRI30               %u\n" $P2_RX_PRI30
	printf "              PRI29               %u\n" $P2_RX_PRI29
	printf "              PRI28               %u\n" $P2_RX_PRI28
	printf "              PRI27               %u\n" $P2_RX_PRI27
	printf "              PRI26               %u\n" $P2_RX_PRI26
	printf "              PRI25               %u\n" $P2_RX_PRI25
	printf "              PRI24               %u\n" $P2_RX_PRI24
	printf "        P2_RX_DSCP_PRI_MAP4\n"
	printf "              PRI39               %u\n" $P2_RX_PRI39
	printf "              PRI38               %u\n" $P2_RX_PRI38
	printf "              PRI37               %u\n" $P2_RX_PRI37
	printf "              PRI36               %u\n" $P2_RX_PRI36
	printf "              PRI35               %u\n" $P2_RX_PRI35
	printf "              PRI34               %u\n" $P2_RX_PRI34
	printf "              PRI33               %u\n" $P2_RX_PRI33
	printf "              PRI32               %u\n" $P2_RX_PRI32
	printf "        P2_RX_DSCP_PRI_MAP5\n"
	printf "              PRI47               %u\n" $P2_RX_PRI47
	printf "              PRI46               %u\n" $P2_RX_PRI46
	printf "              PRI45               %u\n" $P2_RX_PRI45
	printf "              PRI44               %u\n" $P2_RX_PRI44
	printf "              PRI43               %u\n" $P2_RX_PRI43
	printf "              PRI42               %u\n" $P2_RX_PRI42
	printf "              PRI41               %u\n" $P2_RX_PRI41
	printf "              PRI40               %u\n" $P2_RX_PRI40
	printf "        P2_RX_DSCP_PRI_MAP6\n"
	printf "              PRI55               %u\n" $P2_RX_PRI55
	printf "              PRI54               %u\n" $P2_RX_PRI54
	printf "              PRI53               %u\n" $P2_RX_PRI53
	printf "              PRI52               %u\n" $P2_RX_PRI52
	printf "              PRI51               %u\n" $P2_RX_PRI51
	printf "              PRI50               %u\n" $P2_RX_PRI50
	printf "              PRI49               %u\n" $P2_RX_PRI49
	printf "              PRI48               %u\n" $P2_RX_PRI48
	printf "        P2_RX_DSCP_PRI_MAP7\n"
	printf "              PRI63               %u\n" $P2_RX_PRI63
	printf "              PRI62               %u\n" $P2_RX_PRI62
	printf "              PRI61               %u\n" $P2_RX_PRI61
	printf "              PRI60               %u\n" $P2_RX_PRI60
	printf "              PRI59               %u\n" $P2_RX_PRI59
	printf "              PRI58               %u\n" $P2_RX_PRI58
	printf "              PRI57               %u\n" $P2_RX_PRI57
	printf "              PRI56               %u\n" $P2_RX_PRI56
	printf "        P2_IDLE2LPI\n"
	printf "              P2_IDLE2LPI         %u\n" $P2_IDLE2LPI_VAL
	printf "        P2_LPI2WAKE\n"
	printf "              P2_LPI2WAKE         %u\n" $P2_LPI2WAKE_VAL

	printf "========================================================\n"
}

dump_cpts()
{
	CPTS_IDVER=$((CPTS_BASE + 0x0))
	CPTS_CONTROL=$((CPTS_BASE + 0x4))
	CPTS_TS_PUSH=$((CPTS_BASE + 0xc))
	CPTS_TS_LOAD_VAL=$((CPTS_BASE + 0x10))
	CPTS_TS_LOAD_EN=$((CPTS_BASE + 0x014))
	CPTS_INTSTAT_RAW=$((CPTS_BASE + 0x20))
	CPTS_INTSTAT_MASKED=$((CPTS_BASE + 0x024))
	CPTS_INT_ENABLE=$((CPTS_BASE + 0x028))
	CPTS_EVENT_POP=$((CPTS_BASE + 0x30))
	CPTS_EVENT_LOW=$((CPTS_BASE + 0x034))
	CPTS_EVENT_HIGH=$((CPTS_BASE + 0x038))

	CPTS_IDVER_VAL=`devmem $CPTS_IDVER`
	CPTS_CONTROL_VAL=`devmem $CPTS_CONTROL`
	CPTS_TS_PUSH_VAL=`devmem $CPTS_TS_PUSH`
	CPTS_TS_LOAD_VAL_VAL=`devmem $CPTS_TS_LOAD_VAL`
	CPTS_TS_LOAD_EN_VAL=`devmem $CPTS_TS_LOAD_EN`
	CPTS_INTSTAT_RAW_VAL=`devmem $CPTS_INTSTAT_RAW`
	CPTS_INTSTAT_MASKED_VAL=`devmem $CPTS_INTSTAT_MASKED`
	CPTS_INT_ENABLE_VAL=`devmem $CPTS_INT_ENABLE`
	CPTS_EVENT_POP_VAL=`devmem $CPTS_EVENT_POP`
	CPTS_EVENT_LOW_VAL=`devmem $CPTS_EVENT_LOW`
	CPTS_EVENT_HIGH_VAL=`devmem $CPTS_EVENT_HIGH`

	HW4_TS_PUSH_EN=$((($CPTS_CONTROL_VAL & 0x00000800) >> 11))
	HW3_TS_PUSH_EN=$((($CPTS_CONTROL_VAL & 0x00000400) >> 10))
	HW2_TS_PUSH_EN=$((($CPTS_CONTROL_VAL & 0x00000200) >> 9))
	HW1_TS_PUSH_EN=$((($CPTS_CONTROL_VAL & 0x00000100) >> 8))
	INT_TEST=$((($CPTS_CONTROL_VAL & 0x00000002) >> 1))
	CPTS_EN=$(($CPTS_CONTROL_VAL & 0x00000001))
	TS_PUSH=$(($CPTS_TS_PUSH_VAL & 0x00000001))
	TS_LOAD_EN=$(($CPTS_TS_LOAD_EN_VAL & 0x00000001))
	TS_PEND_RAW=$(($CPTS_INTSTAT_RAW_VAL & 0x00000001))
	TS_PEND=$(($CPTS_INTSTAT_MASKED_VAL & 0x00000001))
	TS_PEND_EN=$(($CPTS_INT_ENABLE_VAL & 0x00000001))
	PORT_NUMBER=$((($CPTS_EVENT_HIGH_VAL & 0x1f000000) >> 24))
	EVENT_TYPE=$((($CPTS_EVENT_HIGH_VAL & 0x00f00000) >> 20))
	MESSAGE_TYPE=$((($CPTS_EVENT_HIGH_VAL & 0x000f0000) >> 16))
	SEQUENCE_ID=$(($CPTS_EVENT_HIGH_VAL & 0x0000ffff))
	
	printf "==================-CPTS================================-\n"

	printf "        CPTS_IDVER                  0x%08x\n" $CPTS_IDVER_VAL
	printf "              REVISION              0x%08x\n" $CPTS_IDVER_VAL
	printf "        CPTS_CONTROL                0x%08x\n" $CPTS_CONTROL_VAL
	printf "              HW4_TS_PUSH_EN        %u\n" $HW4_TS_PUSH_EN
	printf "              HW3_TS_PUSH_EN        %u\n" $HW3_TS_PUSH_EN
	printf "              HW2_TS_PUSH_EN        %u\n" $HW2_TS_PUSH_EN
	printf "              HW1_TS_PUSH_EN        %u\n" $HW1_TS_PUSH_EN
	printf "              INT_TEST              %u\n" $INT_TEST
	printf "              CPTS_EN               %u\n" $CPTS_EN
	printf "        CPTS_TS_PUSH                0x%08x\n" $CPTS_TS_PUSH_VAL
	printf "              TS_PUSH               %u\n" $TS_PUSH
	printf "        CPTS_TS_LOAD_VAL            0x%08x\n" $CPTS_TS_LOAD_VAL_VAL
	printf "              TS_LOAD_VAL           %u\n" $CPTS_TS_LOAD_VAL_VAL
	printf "        CPTS_TS_LOAD_EN             0x%08x\n" $CPTS_TS_LOAD_EN_VAL
	printf "              TS_LOAD_EN            %u\n" $TS_LOAD_EN
	printf "        CPTS_INTSTAT_RAW            0x%08x\n" $CPTS_INTSTAT_RAW_VAL
	printf "              TS_PEND_RAW           %u\n" $TS_PEND_RAW
	printf "        CPTS_INTSTAT_MASKED         0x%08x\n" $CPTS_INTSTAT_MASKED_VAL
	printf "              TS_PEND               %u\n" $TS_PEND
	printf "        CPTS_INT_ENABLE             0x%08x\n" $CPTS_INT_ENABLE_VAL
	printf "              TS_PEND_EN            %u\n" $TS_PEND_EN
	printf "        CPTS_EVENT_POP              0x%08x\n" $CPTS_EVENT_POP_VAL
	printf "              EVENT_POP             %u\n" $CPTS_EVENT_POP_VAL
	printf "        CPTS_EVENT_LOW              0x%08x\n" $CPTS_EVENT_LOW_VAL
	printf "              TIME_STAMP            %u\n" $CPTS_EVENT_LOW_VAL
	printf "        CPTS_EVENT_HIGH             0x%08x\n" $CPTS_EVENT_HIGH_VAL
	printf "              PORT_NUMBER           %u\n" $PORT_NUMBER
	printf "              EVENT_TYPE            %u\n" $EVENT_TYPE
	printf "              MESSAGE_TYPE          %u\n" $MESSAGE_TYPE
	printf "              SEQUENCE_ID           %u\n" $SEQUENCE_ID

	printf "========================================================\n"
}

dump_ale()
{
	ALE_IDVER=$((ALE_BASE + 0x0))
	ALE_CONTROL=$((ALE_BASE + 0x08))
	ALE_PRESCALE=$((ALE_BASE + 0x10))
	ALE_UNKNOWN_VLAN=$((ALE_BASE + 0x018))
	ALE_TBLCTL=$((ALE_BASE + 0x20))
	ALE_TBLW2=$((ALE_BASE + 0x034))
	ALE_TBLW1=$((ALE_BASE + 0x038))
	ALE_TBLW0=$((ALE_BASE + 0x03c))
	ALE_PORTCTL0=$((ALE_BASE + 0x40))
	ALE_PORTCTL1=$((ALE_BASE + 0x044))
	ALE_PORTCTL2=$((ALE_BASE + 0x048))
	ALE_PORTCTL3=$((ALE_BASE + 0x04c))
	ALE_PORTCTL4=$((ALE_BASE + 0x50))
	ALE_PORTCTL5=$((ALE_BASE + 0x054))

	ALE_IDVER_VAL_VAL=`devmem $ALE_IDVER`

	ALE_CONTROL_VAL=`devmem $ALE_CONTROL`
	ENABLE_ALE=$((($ALE_CONTROL_VAL & 0x80000000) >> 31))
	CLEAR_TABLE=$((($ALE_CONTROL_VAL & 0x40000000) >> 30))
	AGE_OUT_NOW=$((($ALE_CONTROL_VAL & 0x20000800) >> 29))
	EN_P0_UNI_FLOOD=$((($ALE_CONTROL_VAL & 0x00000100) >> 8))
	LEARN_NO_VID=$((($ALE_CONTROL_VAL & 0x00000080) >> 7))
	EN_VID0_MODE=$((($ALE_CONTROL_VAL & 0x00000040) >> 6))
	ENABLE_OUI_DENY=$((($ALE_CONTROL_VAL & 0x00000020) >> 5))
	BYPASS=$((($ALE_CONTROL_VAL & 0x00000010) >> 4))
	RATE_LIMIT_TX=$((($ALE_CONTROL_VAL & 0x00000008) >> 3))
	VLAN_AWARE=$((($ALE_CONTROL_VAL & 0x00000004) >> 2))
	ENABLE_AUTH_MODE=$((($ALE_CONTROL_VAL & 0x00000002) >> 1))
	ENABLE_RATE_LIMIT=$(($ALE_CONTROL_VAL & 0x00000001))

	ALE_PRESCALE_VAL=`devmem $ALE_PRESCALE`
	PRESCALE=$(($ALE_PRESCALE_VAL & 0x000fffff))

	ALE_UNKNOWN_VLAN_VAL=`devmem $ALE_UNKNOWN_VLAN`
	UNKNOWN_FORCE_UNTAGGED_EGRESS=$((($ALE_UNKNOWN_VLAN_VAL & 0x3f000000) >> 24))
	UNKNOWN_REG_MCAST_FLOOD_MASK=$((($ALE_UNKNOWN_VLAN_VAL & 0x003f0000) >> 16))
	UNKNOWN_MCAST_FLOOD_MASK=$((($ALE_UNKNOWN_VLAN_VAL & 0x00003f00) >> 8))
	UNKNOWN_VLAN_MEMBER_LIST=$(($ALE_UNKNOWN_VLAN_VAL & 0x0000003f))

	ALE_TBLCTL_VAL=`devmem $ALE_TBLCTL`
	WRITE_RDZ=$((($ALE_TBLCTL_VAL & 0x80000000) >> 31))
	ENTRY_POINTER=$(($ALE_TBLCTL_VAL & 0x000003ff))

	ALE_TBLW2_VAL=`devmem $ALE_TBLW2`
	ENTRY71_64=$(($ALE_TBLW2_VAL & 0x000000ff))

	ALE_TBLW1_VAL=`devmem $ALE_TBLW1`
	ALE_TBLW0_VAL=`devmem $ALE_TBLW0`
	
	ALE_PORTCTL0_VAL=`devmem $ALE_PORTCTL0`
	P0_BCAST_LIMIT=$((($ALE_PORTCTL0_VAL & 0xff000000) >> 24))
	P0_MCAST_LIMIT=$((($ALE_PORTCTL0_VAL & 0x00ff0000) >> 16))
	P0_NO_SA_UPDATE=$((($ALE_PORTCTL0_VAL & 0x00000020) >> 5))
	P0_NO_LEARN=$((($ALE_PORTCTL0_VAL & 0x00000010) >> 4))
	P0_VID_INGRESS_CHECK=$((($ALE_PORTCTL0_VAL & 0x00000008) >> 3))
	P0_DROP_UNTAGGED=$((($ALE_PORTCTL0_VAL & 0x00000004) >> 2))
	P0_PORT_STATE=$(($ALE_PORTCTL0_VAL & 0x00000003))
	
	ALE_PORTCTL1_VAL=`devmem $ALE_PORTCTL1`
	P1_BCAST_LIMIT=$((($ALE_PORTCTL1_VAL & 0xff000000) >> 24))
	P1_MCAST_LIMIT=$((($ALE_PORTCTL1_VAL & 0x00ff0000) >> 16))
	P1_NO_SA_UPDATE=$((($ALE_PORTCTL1_VAL & 0x00000020) >> 5))
	P1_NO_LEARN=$((($ALE_PORTCTL1_VAL & 0x00000010) >> 4))
	P1_VID_INGRESS_CHECK=$((($ALE_PORTCTL1_VAL & 0x00000008) >> 3))
	P1_DROP_UNTAGGED=$((($ALE_PORTCTL1_VAL & 0x00000004) >> 2))
	P1_PORT_STATE=$(($ALE_PORTCTL1_VAL & 0x00000003))

	ALE_PORTCTL2_VAL=`devmem $ALE_PORTCTL2`
	P2_BCAST_LIMIT=$((($ALE_PORTCTL2_VAL & 0xff000000) >> 24))
	P2_MCAST_LIMIT=$((($ALE_PORTCTL2_VAL & 0x00ff0000) >> 16))
	P2_NO_SA_UPDATE=$((($ALE_PORTCTL2_VAL & 0x00000020) >> 5))
	P2_NO_LEARN=$((($ALE_PORTCTL2_VAL & 0x00000010) >> 4))
	P2_VID_INGRESS_CHECK=$((($ALE_PORTCTL2_VAL & 0x00000008) >> 3))
	P2_DROP_UNTAGGED=$((($ALE_PORTCTL2_VAL & 0x00000004) >> 2))
	P2_PORT_STATE=$(($ALE_PORTCTL2_VAL & 0x00000003))

	ALE_PORTCTL3_VAL=`devmem $ALE_PORTCTL3`
	P3_BCAST_LIMIT=$((($ALE_PORTCTL3_VAL & 0xff000000) >> 24))
	P3_MCAST_LIMIT=$((($ALE_PORTCTL3_VAL & 0x00ff0000) >> 16))
	P3_NO_SA_UPDATE=$((($ALE_PORTCTL3_VAL & 0x00000020) >> 5))
	P3_NO_LEARN=$((($ALE_PORTCTL3_VAL & 0x00000010) >> 4))
	P3_VID_INGRESS_CHECK=$((($ALE_PORTCTL3_VAL & 0x00000008) >> 3))
	P3_DROP_UNTAGGED=$((($ALE_PORTCTL3_VAL & 0x00000004) >> 2))
	P3_PORT_STATE=$(($ALE_PORTCTL3_VAL & 0x00000003))

	ALE_PORTCTL4_VAL=`devmem $ALE_PORTCTL4`
	P4_BCAST_LIMIT=$((($ALE_PORTCTL4_VAL & 0xff000000) >> 24))
	P4_MCAST_LIMIT=$((($ALE_PORTCTL4_VAL & 0x00ff0000) >> 16))
	P4_NO_SA_UPDATE=$((($ALE_PORTCTL4_VAL & 0x00000020) >> 5))
	P4_NO_LEARN=$((($ALE_PORTCTL4_VAL & 0x00000010) >> 4))
	P4_VID_INGRESS_CHECK=$((($ALE_PORTCTL4_VAL & 0x00000008) >> 3))
	P4_DROP_UNTAGGED=$((($ALE_PORTCTL4_VAL & 0x00000004) >> 2))
	P4_PORT_STATE=$(($ALE_PORTCTL4_VAL & 0x00000003))

	ALE_PORTCTL5_VAL=`devmem $ALE_PORTCTL5`
	P5_BCAST_LIMIT=$((($ALE_PORTCTL5_VAL & 0xff000000) >> 24))
	P5_MCAST_LIMIT=$((($ALE_PORTCTL5_VAL & 0x00ff0000) >> 16))
	P5_NO_SA_UPDATE=$((($ALE_PORTCTL5_VAL & 0x00000020) >> 5))
	P5_NO_LEARN=$((($ALE_PORTCTL5_VAL & 0x00000010) >> 4))
	P5_VID_INGRESS_CHECK=$((($ALE_PORTCTL5_VAL & 0x00000008) >> 3))
	P5_DROP_UNTAGGED=$((($ALE_PORTCTL5_VAL & 0x00000004) >> 2))
	P5_PORT_STATE=$(($ALE_PORTCTL5_VAL & 0x00000003))
	
	printf "====================ALE================================-\n"

	printf "        ALE_IDVER                           0x%08x\n" $ALE_IDVER_VAL
	printf "              REVISION                      0x%08x\n" $ALE_IDVER_VAL
	printf "        ALE_CONTROL                         0x%08x\n" $ALE_CONTROL_VAL
	printf "              ENABLE_ALE                    %u\n" $ENABLE_ALE
	printf "              CLEAR_TABLE                   %u\n" $CLEAR_TABLE
	printf "              AGE_OUT_NOW                   %u\n" $AGE_OUT_NOW
	printf "              EN_P0_UNI_FLOOD               %u\n" $EN_P0_UNI_FLOOD
	printf "              LEARN_NO_VID                  %u\n" $LEARN_NO_VID
	printf "              EN_VID0_MODE                  %u\n" $EN_VID0_MODE
	printf "              ENABLE_OUI_DENY               %u\n" $ENABLE_OUI_DENY
	printf "              BYPASS                        %u\n" $BYPASS
	printf "              RATE_LIMIT_TX                 %u\n" $RATE_LIMIT_TX
	printf "              VLAN_AWARE                    %u\n" $VLAN_AWARE
	printf "              ENABLE_AUTH_MODE              %u\n" $ENABLE_AUTH_MODE
	printf "              ENABLE_RATE_LIMIT             %u\n" $ENABLE_RATE_LIMIT
	printf "        ALE_PRESCALE                        0x%08x\n" $ALE_PRESCALE_VAL
	printf "              PRESCALE                      %u\n" $PRESCALE
	printf "        ALE_UNKNOWN_VLAN                    0x%08x\n" $ALE_UNKNOWN_VLAN_VAL
	printf "              UNKNOWN_FORCE_UNTAGGED_EGRESS %u\n" $UNKNOWN_FORCE_UNTAGGED_EGRESS
	printf "              UNKNOWN_REG_MCAST_FLOOD_MASK  %u\n" $UNKNOWN_REG_MCAST_FLOOD_MASK
	printf "              UNKNOWN_MCAST_FLOOD_MASK      %u\n" $UNKNOWN_MCAST_FLOOD_MASK
	printf "              UNKNOWN_VLAN_MEMBER_LIST      %u\n" $UNKNOWN_VLAN_MEMBER_LIST
	printf "        ALE_TBLCTL                          0x%08x\n" $ALE_TBLCTL_VAL
	printf "              WRITE_RDZ                     %u\n" $WRITE_RDZ
	printf "              ENTRY_POINTER                 %u\n" $ENTRY_POINTER
	printf "        ALE_TBLW2                           0x%08x\n" $ALE_TBLW2_VAL
	printf "              ENTRY71_64                    0x%08x\n" $ENTRY71_64
	printf "        ALE_TBLW1                           0x%08x\n" $ALE_TBLW1_VAL
	printf "              ENTRY63_32                    0x%08x\n" $ALE_TBLW1_VAL
	printf "        ALE_TBLW0                           0x%08x\n" $ALE_TBLW0_VAL
	printf "              ENTRY31_0                     0x%08x\n" $ALE_TBLW0_VAL
	printf "        ALE_PORTCTL0                        0x%08x\n" $ALE_PORTCTL0_VAL
	printf "              BCAST_LIMIT                   %u\n" $P0_BCAST_LIMIT
	printf "              MCAST_LIMIT                   %u\n" $P0_MCAST_LIMIT
	printf "              NO_SA_UPDATE                  %u\n" $P0_NO_SA_UPDATE
	printf "              NO_LEARN                      %u\n" $P0_NO_LEARN
	printf "              VID_INGRESS_CHECK             %u\n" $P0_VID_INGRESS_CHECK
	printf "              DROP_UNTAGGED                 %u\n" $P0_DROP_UNTAGGED
	printf "              PORT_STATE                    %u\n" $P0_PORT_STATE
	printf "        ALE_PORTCTL1                        0x%08x\n" $ALE_PORTCTL1_VAL
	printf "              BCAST_LIMIT                   %u\n" $P1_BCAST_LIMIT
	printf "              MCAST_LIMIT                   %u\n" $P1_MCAST_LIMIT
	printf "              NO_SA_UPDATE                  %u\n" $P1_NO_SA_UPDATE
	printf "              NO_LEARN                      %u\n" $P1_NO_LEARN
	printf "              VID_INGRESS_CHECK             %u\n" $P1_VID_INGRESS_CHECK
	printf "              DROP_UNTAGGED                 %u\n" $P1_DROP_UNTAGGED
	printf "              PORT_STATE                    %u\n" $P1_PORT_STATE
	printf "        ALE_PORTCTL2                        0x%08x\n" $ALE_PORTCTL2_VAL
	printf "              BCAST_LIMIT                   %u\n" $P2_BCAST_LIMIT
	printf "              MCAST_LIMIT                   %u\n" $P2_MCAST_LIMIT
	printf "              NO_SA_UPDATE                  %u\n" $P2_NO_SA_UPDATE
	printf "              NO_LEARN                      %u\n" $P2_NO_LEARN
	printf "              VID_INGRESS_CHECK             %u\n" $P2_VID_INGRESS_CHECK
	printf "              DROP_UNTAGGED                 %u\n" $P2_DROP_UNTAGGED
	printf "              PORT_STATE                    %u\n" $P2_PORT_STATE
	printf "        ALE_PORTCTL3                        0x%08x\n" $ALE_PORTCTL3_VAL
	printf "              BCAST_LIMIT                   %u\n" $P3_BCAST_LIMIT
	printf "              MCAST_LIMIT                   %u\n" $P3_MCAST_LIMIT
	printf "              NO_SA_UPDATE                  %u\n" $P3_NO_SA_UPDATE
	printf "              NO_LEARN                      %u\n" $P3_NO_LEARN
	printf "              VID_INGRESS_CHECK             %u\n" $P3_VID_INGRESS_CHECK
	printf "              DROP_UNTAGGED                 %u\n" $P3_DROP_UNTAGGED
	printf "              PORT_STATE                    %u\n" $P3_PORT_STATE
	printf "        ALE_PORTCTL4                        0x%08x\n" $ALE_PORTCTL4_VAL
	printf "              BCAST_LIMIT                   %u\n" $P4_BCAST_LIMIT
	printf "              MCAST_LIMIT                   %u\n" $P4_MCAST_LIMIT
	printf "              NO_SA_UPDATE                  %u\n" $P4_NO_SA_UPDATE
	printf "              NO_LEARN                      %u\n" $P4_NO_LEARN
	printf "              VID_INGRESS_CHECK             %u\n" $P4_VID_INGRESS_CHECK
	printf "              DROP_UNTAGGED                 %u\n" $P4_DROP_UNTAGGED
	printf "              PORT_STATE                    %u\n" $P4_PORT_STATE
	printf "        ALE_PORTCTL5                        0x%08x\n" $ALE_PORTCTL5_VAL
	printf "              BCAST_LIMIT                   %u\n" $P5_BCAST_LIMIT
	printf "              MCAST_LIMIT                   %u\n" $P5_MCAST_LIMIT
	printf "              NO_SA_UPDATE                  %u\n" $P5_NO_SA_UPDATE
	printf "              NO_LEARN                      %u\n" $P5_NO_LEARN
	printf "              VID_INGRESS_CHECK             %u\n" $P5_VID_INGRESS_CHECK
	printf "              DROP_UNTAGGED                 %u\n" $P5_DROP_UNTAGGED
	printf "              PORT_STATE                    %u\n" $P5_PORT_STATE

	printf "========================================================\n"
}

dump_sl()
{
	sl_name=$1
	sl_base=$2

	SL_IDVER=$((sl_base + 0x0))
	SL_MACCONTROL=$((sl_base + 0x04))
	SL_MACSTATUS=$((sl_base + 0x08))
	SL_SOFT_RESET=$((sl_base + 0x0c))
	SL_RX_MAXLEN=$((sl_base + 0x10))
	SL_BOFFTEST=$((sl_base + 0x014))
	SL_RX_PAUSE=$((sl_base + 0x018))
	SL_TX_PAUSE=$((sl_base + 0x01c))
	SL_EMCONTROL=$((sl_base + 0x20))
	SL_RX_PRI_MAP=$((sl_base + 0x024))
	SL_TX_GAP=$((sl_base + 0x028))
	
	SL_IDVER_VAL=`devmem $SL_IDVER`
	SL_MACCONTROL_VAL=`devmem $SL_MACCONTROL`
	RX_CMF_EN=$((($SL_MACCONTROL_VAL & 0x01000000) >> 24))
	RX_CSF_EN=$((($SL_MACCONTROL_VAL & 0x00800000) >> 23))
	RX_CEF_EN=$((($SL_MACCONTROL_VAL & 0x00400000) >> 22))
	TX_SHORT_GAP_LIM_EN=$((($SL_MACCONTROL_VAL & 0x00200000) >> 21))
	EXT_EN=$((($SL_MACCONTROL_VAL & 0x00040000) >> 18))
	GIG_FORCE=$((($SL_MACCONTROL_VAL & 0x00020000) >> 17))
	IFCTL_B=$((($SL_MACCONTROL_VAL & 0x00010000) >> 16))
	IFCTL_A=$((($SL_MACCONTROL_VAL & 0x00008000) >> 15))
	CMD_IDLE=$((($SL_MACCONTROL_VAL & 0x00000800) >> 11))
	TX_SHORT_GAP_EN=$((($SL_MACCONTROL_VAL & 0x00000400) >> 10))
	GIG=$((($SL_MACCONTROL_VAL & 0x00000080) >> 7))
	TX_PACE=$((($SL_MACCONTROL_VAL & 0x00000040) >> 6))
	GMII_EN=$((($SL_MACCONTROL_VAL & 0x00000020) >> 5))
	TX_FLOW_EN=$((($SL_MACCONTROL_VAL & 0x00000010) >> 4))
	RX_FLOW_EN=$((($SL_MACCONTROL_VAL & 0x00000008) >> 3))
	MTEST=$((($SL_MACCONTROL_VAL & 0x00000004) >> 2))
	LOOPBACK=$((($SL_MACCONTROL_VAL & 0x00000002) >> 1))
	FULLDUPLEX=$(($SL_MACCONTROL_VAL & 0x00000001))

	SL_MACSTATUS_VAL=`devmem $SL_MACSTATUS`
	IDLE=$((($SL_MACSTATUS_VAL & 0x80000000) >> 31))
	EXT_GIG=$((($SL_MACSTATUS_VAL & 0x00000010) >> 4))
	RX_CMF_EN=$((($SL_MACSTATUS_VAL & 0x00000008) >> 3))
	RX_CMF_EN=$((($SL_MACSTATUS_VAL & 0x00000002) >> 1))
	FULLDUPLEX=$(($SL_MACSTATUS_VAL & 0x00000001))

	SL_SOFT_RESET_VAL=`devmem $SL_SOFT_RESET`
	SOFT_RESET=$(($SL_SOFT_RESET_VAL & 0x00000001))
	
	SL_RX_MAXLEN_VAL=`devmem $SL_RX_MAXLEN`
	RX_MAXLEN=$(($SL_RX_MAXLEN_VAL & 0x00003fff))
	
	SL_BOFFTEST_VAL=`devmem $SL_BOFFTEST`
	PACEVAL=$((($SL_BOFFTEST_VAL & 0x7c000000) >> 26))
	RNDNUM=$((($SL_BOFFTEST_VAL & 0x03ff0000) >> 16))
	COLL_COUNT=$((($SL_BOFFTEST_VAL & 0x0000f000) >> 12))
	TX_BACKOFF=$(($SL_BOFFTEST_VAL & 0x000003ff))

	SL_RX_PAUSE_VAL=`devmem $SL_RX_PAUSE`
	RX_PAUSETIMER=$(($SL_RX_PAUSE_VAL & 0x0000ffff))
	
	SL_TX_PAUSE_VAL=`devmem $SL_TX_PAUSE`
	TX_PAUSETIMER=$(($SL_TX_PAUSE_VAL & 0x0000ffff))

	SL_EMCONTROL_VAL=`devmem $SL_EMCONTROL`
	EMCONTROL_SOFT=$((($SL_EMCONTROL_VAL & 0x00000002) >> 1))
	EMCONTROL_FREE=$(($SL_EMCONTROL_VAL & 0x00000001))
	
	SL_RX_PRI_MAP_VAL=`devmem $SL_RX_PRI_MAP`
	SL_RX_PRI7=$((($SL_RX_PRI_MAP & 0x70000000) >> 28))
	SL_RX_PRI6=$((($SL_RX_PRI_MAP & 0x07000000) >> 24))
	SL_RX_PRI5=$((($SL_RX_PRI_MAP & 0x00700000) >> 20))
	SL_RX_PRI4=$((($SL_RX_PRI_MAP & 0x00070000) >> 16))
	SL_RX_PRI3=$((($SL_RX_PRI_MAP & 0x00007000) >> 12))
	SL_RX_PRI2=$((($SL_RX_PRI_MAP & 0x00000700) >> 8))
	SL_RX_PRI1=$((($SL_RX_PRI_MAP & 0x00000070) >> 4))
	SL_RX_PRI0_FREE=$(($SL_RX_PRI_MAP & 0x00000007))
	
	SL_TX_GAP_VAL=`devmem $SL_TX_GAP`
	TX_GAP=$(($SL_TX_GAP_VAL & 0x000001ff))
	
	printf "==================%s================================-\n" $sl_name
	
	printf "        SL_IDVER                    0x%08x\n" $SL_IDVER_VAL
	printf "              REVISION              %u\n" $SL_IDVER_VAL
	printf "        SL_MACCONTROL               0x%08x\n" $SL_MACCONTROL_VAL
	printf "              RX_CMF_EN             %u\n" $RX_CMF_EN
	printf "              RX_CSF_EN             %u\n" $RX_CSF_EN
	printf "              RX_CEF_EN             %u\n" $RX_CEF_EN
	printf "              TX_SHORT_GAP_LIM_EN   %u\n" $TX_SHORT_GAP_LIM_EN
	printf "              EXT_EN                %u\n" $EXT_EN
	printf "              GIG_FORCE             %u\n" $GIG_FORCE
	printf "              IFCTL_B               %u\n" $IFCTL_B
	printf "              IFCTL_A               %u\n" $IFCTL_A
	printf "              CMD_IDLE              %u\n" $CMD_IDLE
	printf "              TX_SHORT_GAP_EN       %u\n" $TX_SHORT_GAP_EN
	printf "              GIG                   %u\n" $GIG
	printf "              TX_PACE               %u\n" $TX_PACE
	printf "              GMII_EN               %u\n" $GMII_EN
	printf "              TX_FLOW_EN            %u\n" $TX_FLOW_EN
	printf "              RX_FLOW_EN            %u\n" $RX_FLOW_EN
	printf "              MTEST                 %u\n" $MTEST
	printf "              LOOPBACK              %u\n" $LOOPBACK
	printf "              FULLDUPLEX            %u\n" $FULLDUPLEX
	printf "        SL_MACSTATUS                0x%08x\n" $SL_MACSTATUS_VAL
	printf "              IDLE                  %u\n" $IDLE
	printf "              EXT_GIG               %u\n" $EXT_GIG
	printf "              EXT_FULLDUPLEX        %u\n" $EXT_FULLDUPLEX
	printf "              RX_FLOW_ACT           %u\n" $RX_FLOW_ACT
	printf "              TX_FLOW_ACT           %u\n" $TX_FLOW_ACT
	printf "        SL_SOFT_RESET               0x%08x\n" $SL_SOFT_RESET_VAL
	printf "              SOFT_RESET            %u\n" $SOFT_RESET
	printf "        SL_RX_MAXLEN                0x%08x\n" $SL_RX_MAXLEN_VAL
	printf "              RX_MAXLEN             %u\n" $RX_MAXLEN
	printf "        SL_BOFFTEST                 0x%08x\n" $SL_BOFFTEST_VAL
	printf "              PACEVAL               %u\n" $PACEVAL
	printf "              RNDNUM                %u\n" $RNDNUM
	printf "              COLL_COUNT            %u\n" $COLL_COUNT
	printf "              TX_BACKOFF            %u\n" $TX_BACKOFF
	printf "        SL_RX_PAUSE                 0x%08x\n" $SL_RX_PAUSE_VAL
	printf "              RX_PAUSETIMER         %u\n" $RX_PAUSETIMER
	printf "        SL_TX_PAUSE                 0x%08x\n" $SL_TX_PAUSE_VAL
	printf "              TX_PAUSETIMER         %u\n" $TX_PAUSETIMER
	printf "        SL_EMCONTROL                0x%08x\n" $SL_EMCONTROL_VAL
	printf "              SOFT                  %u\n" $EMCONTROL_SOFT
	printf "              FREE                  %u\n" $EMCONTROL_FREE
	printf "        SL_RX_PRI_MAP               0x%08x\n" $SL_RX_PRI_MAP_VAL
	printf "              PRI7                  %u\n" $SL_RX_PRI7
	printf "              PRI6                  %u\n" $SL_RX_PRI6
	printf "              PRI5                  %u\n" $SL_RX_PRI5
	printf "              PRI4                  %u\n" $SL_RX_PRI4
	printf "              PRI3                  %u\n" $SL_RX_PRI3
	printf "              PRI2                  %u\n" $SL_RX_PRI2
	printf "              PRI1                  %u\n" $SL_RX_PRI1
	printf "              PRI0                  %u\n" $SL_RX_PRI0
	printf "        SL_TX_GAP                   0x%08x\n" $SL_TX_GAP_VAL
	printf "              TX_GAP                %u\n" $TX_GAP

	printf "========================================================\n"
}

dump_mdio()
{
	MDIO_VER=$((MDIO_BASE + 0x0))
	MDIO_CONTROL=$((MDIO_BASE + 0x04))
	MDIO_ALIVE=$((MDIO_BASE + 0x08))
	MDIO_LINK=$((MDIO_BASE + 0x0c))
	MDIO_LINKINTRAW=$((MDIO_BASE + 0x10))
	MDIO_LINKINTMASKED=$((MDIO_BASE + 0x014))
	MDIO_USERINTRAW=$((MDIO_BASE + 0x20))
	MDIO_USERINTMASKED=$((MDIO_BASE + 0x024))
	MDIO_USERINTMASKSET=$((MDIO_BASE + 0x028))
	MDIO_USERINTMASKCLR=$((MDIO_BASE + 0x02c))
	MDIO_USERACCESS0=$((MDIO_BASE + 0x80))
	MDIO_USERPHYSEL0=$((MDIO_BASE + 0x084))
	MDIO_USERACCESS1=$((MDIO_BASE + 0x088))
	MDIO_USERPHYSEL1=$((MDIO_BASE + 0x08c))

	MDIO_VER_VAL=`devmem $MDIO_VER`

	MDIO_CONTROL_VAL=`devmem $MDIO_CONTROL`
	MDIO_IDLE=$((($MDIO_CONTROL_VAL & 0x80000000) >> 31))
	MDIO_ENABLE=$((($MDIO_CONTROL_VAL & 0x40000000) >> 30))
	HIGHEST_USER_CHANNEL=$((($MDIO_CONTROL_VAL & 0x1f000000) >> 24))
	MDIO_PREAMBLE=$((($MDIO_CONTROL_VAL & 0x00100000) >> 20))
	MDIO_FAULT=$((($MDIO_CONTROL_VAL & 0x00080000) >> 19))
	MDIO_FAULTENB=$((($MDIO_CONTROL_VAL & 0x00040000) >> 18))
	MDIO_INTTESTENB=$((($MDIO_CONTROL_VAL & 0x00020000) >> 17))
	MDIO_CLKDIV=$(($MDIO_CONTROL_VAL & 0x0000ffff))

	MDIO_ALIVE_VAL=`devmem $MDIO_ALIVE`
	MDIO_LINK_VAL=`devmem $MDIO_LINK`

	MDIO_LINKINTRAW_VAL=`devmem $MDIO_LINKINTRAW`
	LINKINTRAW=$(($MDIO_LINKINTRAW_VAL & 0x00000003))
	
	MDIO_LINKINTMASKED_VAL=`devmem $MDIO_LINKINTMASKED`
	LINKINTMASKED=$(($MDIO_LINKINTMASKED_VAL & 0x00000003))
	
	MDIO_USERINTRAW_VAL=`devmem $MDIO_USERINTRAW`
	USERINTRAW=$(($MDIO_USERINTRAW_VAL & 0x00000003))
	
	MDIO_USERINTMASKED_VAL=`devmem $MDIO_USERINTMASKED`
	USERINTMASKED=$(($MDIO_USERINTMASKED_VAL & 0x00000003))
	
	MDIO_USERINTMASKSET_VAL=`devmem $MDIO_USERINTMASKSET`
	USERINTMASKSET=$(($MDIO_USERINTMASKSET_VAL & 0x00000003))
	
	MDIO_USERINTMASKCLR_VAL=`devmem $MDIO_USERINTMASKCLR`
	USERINTMASKCLEAR=$(($MDIO_USERINTMASKCLR_VAL & 0x00000003))
	
	MDIO_USERACCESS0_VAL=`devmem $MDIO_USERACCESS0`
	MDIO0_GO=$((($MDIO_USERACCESS0_VAL & 0x80000000) >> 31))
	MDIO0_WRITE=$((($MDIO_USERACCESS0_VAL & 0x80000000) >> 30))
	MDIO0_ACK=$((($MDIO_USERACCESS0_VAL & 0x40000000) >> 29))
	MDIO0_REGADR=$((($MDIO_USERACCESS0_VAL & 0x03e00000) >> 21))
	MDIO0_PHYADR=$((($MDIO_USERACCESS0_VAL & 0x001f0000) >> 16))
	MDIO0_DATA=$(($MDIO_USERACCESS0_VAL & 0x0000ffff))

	MDIO_USERPHYSEL0_VAL=`devmem $MDIO_USERPHYSEL0`
	MDIO0_LINKSEL=$((($MDIO_USERPHYSEL0_VAL & 0x00000080) >> 7))
	MDIO0_LINKINTENB=$((($MDIO_USERPHYSEL0_VAL & 0x00000040) >> 6))
	MDIO0_PHYADDRMON=$(($MDIO_USERPHYSEL0_VAL & 0x00000001f))
	
	MDIO_USERACCESS1_VAL=`devmem $MDIO_USERACCESS1`
	MDIO1_GO=$((($MDIO_USERACCESS1_VAL & 0x80000000) >> 31))
	MDIO1_WRITE=$((($MDIO_USERACCESS1_VAL & 0x80000000) >> 30))
	MDIO1_ACK=$((($MDIO_USERACCESS1_VAL & 0x40000000) >> 29))
	MDIO1_REGADR=$((($MDIO_USERACCESS1_VAL & 0x03e00000) >> 21))
	MDIO1_PHYADR=$((($MDIO_USERACCESS1_VAL & 0x001f0000) >> 16))
	MDIO1_DATA=$(($MDIO_USERACCESS1_VAL & 0x0000ffff))
	
	MDIO_USERPHYSEL1_VAL=`devmem $MDIO_USERPHYSEL1`
	MDIO1_LINKSEL=$((($MDIO_USERPHYSEL1_VAL & 0x00000080) >> 7))
	MDIO1_LINKINTENB=$((($MDIO_USERPHYSEL1_VAL & 0x00000040) >> 6))
	MDIO1_PHYADDRMON=$(($MDIO_USERPHYSEL1_VAL & 0x00000001f))
	
	printf "====================MDIO================================\n"

	printf "        MDIO_VER                    0x%08x\n" $MDIO_VER_VAL
	printf "              REVISION              %u\n" $MDIO_VER_VAL
	printf "        MDIO_CONTROL                0x%08x\n" $MDIO_CONTROL_VAL
	printf "              IDLE                  %u\n" $MDIO_IDLE
	printf "              ENABLE                %u\n" $MDIO_ENABLE
	printf "              HIGHEST_USER_CHANNEL  %u\n" $HIGHEST_USER_CHANNEL
	printf "              PREAMBLE              %u\n" $MDIO_PREAMBLE
	printf "              FAULT                 %u\n" $MDIO_FAULT
	printf "              FAULTENB              %u\n" $MDIO_FAULTENB
	printf "              INTTESTENB            %u\n" $MDIO_INTTESTENB
	printf "              CLKDIV                %u\n" $MDIO_CLKDIV
	printf "        MDIO_ALIVE                  0x%08x\n" $MDIO_ALIVE_VAL
	printf "              ALIVE                 %u\n" $MDIO_ALIVE_VAL
	printf "        MDIO_LINK                   0x%08x\n" $MDIO_LINK_VAL
	printf "              LINK                  0x%08x\n" $MDIO_LINK_VAL
	printf "        MDIO_LINKINTRAW             0x%08x\n" $MDIO_LINKINTRAW_VAL
	printf "              LINKINTRAW            %u\n" $LINKINTRAW
	printf "        MDIO_LINKINTMASKED          0x%08x\n" $MDIO_LINKINTMASKED_VAL
	printf "              LINKINTMASKED         %u\n" $LINKINTMASKED
	printf "        MDIO_USERINTRAW             0x%08x\n" $MDIO_USERINTRAW_VAL
	printf "              USERINTRAW            %u\n" $USERINTRAW
	printf "        MDIO_USERINTMASKED          0x%08x\n" $MDIO_USERINTMASKED_VAL
	printf "              USERINTMASKED         %u\n" $USERINTMASKED
	printf "        MDIO_USERINTMASKSET         0x%08x\n" $MDIO_USERINTMASKSET_VAL
	printf "              USERINTMASKSET        %u\n" $USERINTMASKSET
	printf "        MDIO_USERINTMASKCLR         0x%08x\n" $MDIO_USERINTMASKCLR_VAL
	printf "              USERINTMASKCLEAR      %u\n" $USERINTMASKCLEAR
	printf "        MDIO_USERACCESS0            0x%08x\n" $MDIO_USERACCESS0_VAL
	printf "              GO                    %u\n" $MDIO0_GO
	printf "              WRITE                 %u\n" $MDIO0_WRITE
	printf "              ACK                   %u\n" $MDIO0_ACK
	printf "              REGADR                %u\n" $MDIO0_REGADR
	printf "              PHYADR                %u\n" $MDIO0_PHYADR
	printf "              DATA                  0x%04x\n" $MDIO0_DATA
	printf "        MDIO_USERPHYSEL0            0x%08x\n" $MDIO_USERPHYSEL0_VAL
	printf "              LINKSEL               %u\n" $MDIO0_LINKSEL
	printf "              LINKINTENB            %u\n" $MDIO0_LINKINTENB
	printf "              LINKINTENB            %u\n" $MDIO0_LINKINTENB
	printf "        MDIO_USERACCESS1            0x%08x\n" $MDIO_USERACCESS1_VAL
	printf "              GO                    %u\n" $MDIO1_GO
	printf "              WRITE                 %u\n" $MDIO1_WRITE
	printf "              ACK                   %u\n" $MDIO1_ACK
	printf "              REGADR                %u\n" $MDIO1_REGADR
	printf "              PHYADR                %u\n" $MDIO1_PHYADR
	printf "              DATA                  0x%04x\n" $MDIO1_DATA
	printf "        MDIO_USERPHYSEL1            0x%08x\n" $MDIO_USERPHYSEL1_VAL
	printf "              LINKSEL               %u\n" $MDIO1_LINKSEL
	printf "              LINKINTENB            %u\n" $MDIO1_LINKINTENB
	printf "              LINKINTENB            %u\n" $MDIO1_LINKINTENB

	printf "========================================================\n"
}

dump_wr()
{
	WR_IDVER=$((WR_BASE + 0x0))
	WR_SOFT_RESET=$((WR_BASE + 0x04))
	WR_CONTROL=$((WR_BASE + 0x08))
	WR_INT_CONTROL=$((WR_BASE + 0x0c))
	WR_C0_RX_THRESH_EN=$((WR_BASE + 0x10))
	WR_C0_RX_EN=$((WR_BASE + 0x014))
	WR_C0_TX_EN=$((WR_BASE + 0x018))
	WR_C0_MISC_EN=$((WR_BASE + 0x01c))
	WR_C0_RX_THRESH_STAT=$((WR_BASE + 0x40))
	WR_C0_RX_STAT=$((WR_BASE + 0x044))
	WR_C0_TX_STAT=$((WR_BASE + 0x048))
	WR_C0_MISC_STAT=$((WR_BASE + 0x04c))
	WR_C0_RX_IMAX=$((WR_BASE + 0x70))
	WR_C0_TX_IMAX=$((WR_BASE + 0x074))
	WR_RGMII_CTL=$((WR_BASE + 0x088))
	WR_STATUS=$((WR_BASE + 0x08c))

	WR_IDVER_VAL=`devmem $WR_IDVER`

	WR_SOFT_RESET_VAL=`devmem $WR_SOFT_RESET`
	SOFT_RESET=$(($WR_SOFT_RESET_VAL & 0x00000001))

	WR_CONTROL_VAL=`devmem $WR_CONTROL`
	SS_EEE_EN=$((($WR_CONTROL_VAL & 0x00000100) >> 8))
	MMR_STDBYMODE=$((($WR_CONTROL_VAL & 0x000000c0) >> 2))
	MMR_IDLEMODE=$(($WR_CONTROL_VAL & 0x00000003))

	WR_INT_CONTROL_VAL=`devmem $WR_INT_CONTROL`
	INT_TEST=$((($WR_INT_CONTROL_VAL & 0x80000000) >> 31))
	INT_PACE_EN=$((($WR_INT_CONTROL_VAL & 0x003f0000) >> 16))
	INT_PRESCALE=$(($WR_INT_CONTROL_VAL & 0x00000fff))
	
	WR_C0_RX_THRESH_EN_VAL=`devmem $WR_C0_RX_THRESH_EN`
	C0_RX_THRESH_EN=$(($WR_C0_RX_THRESH_EN_VAL & 0x000000ff))
	
	WR_C0_RX_EN_VAL=`devmem $WR_C0_RX_EN`
	C0_RX_EN=$(($WR_C0_RX_EN_VAL & 0x000000ff))
	
	WR_C0_TX_EN_VAL=`devmem $WR_C0_TX_EN`
	C0_TX_EN=$(($WR_C0_TX_EN_VAL & 0x000000ff))
	
	WR_C0_MISC_EN_VAL=`devmem $WR_C0_MISC_EN`
	C0_MISC_EN=$(($WR_C0_MISC_EN_VAL & 0x0000001f))
	
	WR_C0_RX_THRESH_STAT_VAL=`devmem $WR_C0_RX_THRESH_STAT`
	C0_RX_THRESH_STAT=$(($WR_C0_RX_THRESH_STAT_VAL & 0x000000ff))
	
	WR_C0_RX_STAT_VAL=`devmem $WR_C0_RX_STAT`
	C0_RX_STAT=$(($WR_C0_RX_STAT_VAL & 0x000000ff))
	
	WR_C0_TX_STAT_VAL=`devmem $WR_C0_TX_STAT`
	C0_TX_STAT=$(($WR_C0_TX_STAT_VAL & 0x000000ff))
	
	WR_C0_MISC_STAT_VAL=`devmem $WR_C0_MISC_STAT`
	C0_MISC_STAT=$(($WR_C0_MISC_STAT_VAL & 0x0000001f))
	
	WR_C0_RX_IMAX_VAL=`devmem $WR_C0_RX_IMAX`
	C0_RX_IMAX=$(($WR_C0_RX_IMAX_VAL & 0x0000003f))
	
	WR_C0_TX_IMAX_VAL=`devmem $WR_C0_TX_IMAX`
	C0_TX_IMAX=$(($WR_C0_TX_IMAX_VAL & 0x0000003f))
	
	WR_RGMII_CTL_VAL=`devmem $WR_RGMII_CTL`
	RGMII2_FULLDUPLEX=$((($WR_RGMII_CTL_VAL & 0x00000080) >> 7))
	RGMII2_SPEED=$((($WR_RGMII_CTL_VAL & 0x00000060) >> 5))
	RGMII2_LINK=$((($WR_RGMII_CTL_VAL & 0x00000010) >> 4))
	RGMII1_FULLDUPLEX=$((($WR_RGMII_CTL_VAL & 0x00000008) >> 3))
	RGMII1_SPEED=$((($WR_RGMII_CTL_VAL & 0x00000006) >> 1))
	RGMII1_LINK=$(($WR_RGMII_CTL_VAL & 0x00000001))

	WR_STATUS_VAL=`devmem $WR_STATUS`
	SPF2_CLKSTOP_ACK=$((($WR_STATUS_VAL & 0x00000004) >> 2))
	SPF1_CLKSTOP_ACK=$((($WR_STATUS_VAL & 0x00000002) >> 1))
	EEE_CLKSTOP_ACK=$(($WR_STATUS_VAL & 0x00000001))
	
	printf "=====================WR================================-\n"

	printf "        WR_IDVER                    0x%08x\n" $WR_IDVER_VAL
	printf "              REVISION              %u\n" $WR_IDVER_VAL
	printf "        WR_SOFT_RESET               0x%08x\n" $WR_SOFT_RESET_VAL
	printf "              SOFT_RESET            %u\n" $SOFT_RESET
	printf "        WR_CONTROL                  0x%08x\n" $WR_CONTROL_VAL
	printf "              SS_EEE_EN             %u\n" $SS_EEE_EN
	printf "              MMR_STDBYMODE         %u\n" $MMR_STDBYMODE
	printf "              MMR_IDLEMODE          %u\n" $MMR_IDLEMODE
	printf "        WR_INT_CONTROL              0x%08x\n" $WR_INT_CONTROL_VAL
	printf "              INT_TEST              %u\n" $INT_TEST
	printf "              INT_PACE_EN           0x%02x\n" $INT_PACE_EN
	printf "              INT_PRESCALE          %u\n" $INT_PRESCALE
	printf "        WR_C0_RX_THRESH_EN          0x%08x\n" $WR_C0_RX_THRESH_EN_VAL
	printf "              C0_RX_THRESH_EN       0x%02x\n" $C0_RX_THRESH_EN
	printf "        WR_C0_RX_EN                 0x%08x\n" $WR_C0_RX_EN_VAL
	printf "              C0_RX_EN              0x%02x\n" $C0_RX_EN
	printf "        WR_C0_TX_EN                 0x%08x\n" $WR_C0_TX_EN_VAL
	printf "              C0_TX_EN              0x%02x\n" $C0_TX_EN
	printf "        WR_C0_MISC_EN               0x%08x\n" $WR_C0_MISC_EN_VAL
	printf "              C0_MISC_EN            0x%02x\n" $C0_MISC_EN
	printf "        WR_C0_RX_THRESH_STAT        0x%08x\n" $WR_C0_RX_THRESH_STAT_VAL
	printf "              C0_RX_THRESH_STAT     0x%02x\n" $C0_RX_THRESH_STAT
	printf "        WR_C0_RX_STAT               0x%08x\n" $WR_C0_RX_STAT_VAL
	printf "              C0_RX_STAT            0x%02x\n" $C0_RX_STAT
	printf "        WR_C0_TX_STAT               0x%08x\n" $WR_C0_TX_STAT_VAL
	printf "              C0_TX_STAT            0x%02x\n" $C0_TX_STAT
	printf "        WR_C0_MISC_STAT             0x%08x\n" $WR_C0_MISC_STAT_VAL
	printf "              C0_MISC_STAT          0x%02x\n" $C0_MISC_STAT
	printf "        WR_C0_RX_IMAX               0x%08x\n" $WR_C0_RX_IMAX_VAL
	printf "              C0_RX_IMAX            %u\n" $C0_RX_IMAX
	printf "        WR_C0_TX_IMAX               0x%08x\n" $WR_C0_TX_IMAX_VAL
	printf "              C0_TX_IMAX            %u\n" $C0_TX_IMAX
	printf "        WR_RGMII_CTL                0x%08x\n" $WR_RGMII_CTL_VAL
	printf "              RGMII2_FULLDUPLEX     %u\n" $RGMII2_FULLDUPLEX
	printf "              RGMII2_SPEED          %u\n" $RGMII2_SPEED
	printf "              RGMII2_LINK           %u\n" $RGMII2_LINK
	printf "              RGMII1_FULLDUPLEX     %u\n" $RGMII1_FULLDUPLEX
	printf "              RGMII1_SPEED          %u\n" $RGMII1_SPEED
	printf "              RGMII1_LINK           %u\n" $RGMII1_LINK
	printf "        WR_STATUS                   0x%08x\n" $WR_STATUS_VAL
	printf "              SPF2_CLKSTOP_ACK      %u\n" $RGMII2_FULLDUPLEX
	printf "              SPF1_CLKSTOP_ACK      %u\n" $SPF1_CLKSTOP_ACK
	printf "              EEE_CLKSTOP_ACK       %u\n" $EEE_CLKSTOP_ACK

	printf "========================================================\n"
}

dump_spf()
{
	spf_name=$1
	spf_base=$2

	if [ $((spf_base)) -eq 0 ]; then
		return
	fi
	
	SPF_IDVER=$((spf_base + 0x0))
	SPF_STATUS=$((spf_base + 0x04))
	SPF_CONTROL=$((spf_base + 0x08))
	SPF_DROPCOUNT=$((spf_base + 0x0c))
	SPF_SWRESET=$((spf_base + 0x10))
	SPF_PRESCALE=$((spf_base + 0x014))
	SPF_RATELIM0=$((spf_base + 0x018))
	SPF_RATELIM1=$((spf_base + 0x1c))
	SPF_RATELIM2=$((spf_base + 0x20))
	SPF_RATELIM3=$((spf_base + 0x24))
	SPF_CONST0=$((spf_base + 0x028))
	SPF_CONST1=$((spf_base + 0x30))
	SPF_CONST2=$((spf_base + 0x034))
	SPF_CONST3=$((spf_base + 0x038))
	SPF_CONST4=$((spf_base + 0x03c))
	SPF_CONST5=$((spf_base + 0x040))
	SPF_CONST6=$((spf_base + 0x044))
	SPF_CONST7=$((spf_base + 0x48))
	SPF_INSTRW2=$((spf_base + 0x50))
	SPF_INSTRW1=$((spf_base + 0x054))
	SPF_INSTRW0=$((spf_base + 0x058))
	SPF_INSTR_CTL=$((spf_base + 0x05c))
	SPF_LOG_BEGIN=$((spf_base + 0x60))
	SPF_LOG_END=$((spf_base + 0x064))
	SPF_LOG_HWPTR=$((spf_base + 0x068))
	SPF_LOG_SWPTR=$((spf_base + 0x06c))
	SPF_LOG_MAP0=$((spf_base + 0x70))
	SPF_LOG_MAP1=$((spf_base + 0x074))
	SPF_LOG_THRESH0=$((spf_base + 0x078))
	SPF_LOG_THRESH1=$((spf_base + 0x07c))
	SPF_LOG_THRESH2=$((spf_base + 0x80))
	SPF_LOG_THRESH3=$((spf_base + 0x084))
	SPF_LOG_THRESH4=$((spf_base + 0x088))
	SPF_LOG_THRESH5=$((spf_base + 0x08c))
	SPF_LOG_THRESH6=$((spf_base + 0x90))
	SPF_LOG_THRESH7=$((spf_base + 0x094))
	SPF_LOG_THRESH8=$((spf_base + 0x098))
	SPF_INTCNT=$((spf_base + 0x09c))
	SPF_INT_RAW=$((spf_base + 0xa0))
	SPF_INT_MASKED=$((spf_base + 0x0a4))
	SPF_MASK_SET=$((spf_base + 0x0a8))
	SPF_MASK_CLR=$((spf_base + 0x0ac))


	printf "====================%s================================-\n" $spf_name

	val=`devmem $SPF_IDVER`

	printf "        SPF_IDVER                   0x%08x\n" $val
	printf "              REVISION              %u\n" $val

	val=`devmem $SPF_STATUS`
	SPF_BUSY=$(($val & 0x00000001))

	printf "        SPF_STATUS                  0x%08x\n" $val
	printf "              SPF_BUSY              %u\n" $SPF_BUSY

	val=`devmem $SPF_CONTROL`
	SPF_BUSY=$(($val & 0x00000001))
	SPF_LOGOW_EN=$((($val & 0x00000200) >> 9))
	SPF_LOG_EN=$((($val & 0x00000100) >> 8))
	SPF_RULE_LOG=$((($val & 0x00000008) >> 3))
	SPF_EXT_BYPASS=$((($val & 0x00000004) >> 2))
	SPF_DROP=$((($val & 0x00000002) >> 1))
	SPF_ENABLE=$(($val & 0x00000001))

	printf "        SPF_CONTROL                 0x%08x\n" $val
	printf "              SPF_LOGOW_EN          %u\n" $SPF_LOGOW_EN
	printf "              SPF_LOG_EN            %u\n" $SPF_LOG_EN
	printf "              SPF_RULE_LOG          %u\n" $SPF_RULE_LOG
	printf "              SPF_EXT_BYPASS        %u\n" $SPF_EXT_BYPASS
	printf "              SPF_DROP              %u\n" $SPF_DROP
	printf "              SPF_ENABLE            %u\n" $SPF_ENABLE

	val=`devmem $SPF_DROPCOUNT`
	SPF_DROPCNT=$(($val & 0x00ffffff))

	printf "        SPF_DROPCOUNT               0x%08x\n" $val
	printf "              SPF_DROPCNT           %u\n" $SPF_DROPCNT

	val=`devmem $SPF_SWRESET`
	SPF_SWRST=$(($val & 0x00000001))

	printf "        SPF_SWRESET                 0x%08x\n" $val
	printf "              SPF_SWRST             %u\n" $SPF_SWRST

	val=`devmem $SPF_PRESCALE`
	SPF_PRESCALE=$(($val & 0x000fffff))

	printf "        SPF_PRESCALE                0x%08x\n" $val
	printf "              SPF_PRESCALE          %u\n" $SPF_PRESCALE

	val=`devmem $SPF_RATELIM0`
	SPF_RATELIM=$(($val & 0x000000ff))

	printf "        SPF_RATELIM0                0x%08x\n" $val
	printf "              SPF_RATELIM           %u\n" $SPF_RATELIM

	val=`devmem $SPF_RATELIM1`
	SPF_RATELIM=$(($val & 0x000000ff))

	printf "        SPF_RATELIM1                0x%08x\n" $val
	printf "              SPF_RATELIM           %u\n" $SPF_RATELIM

	val=`devmem $SPF_RATELIM2`
	SPF_RATELIM=$(($val & 0x000000ff))

	printf "        SPF_RATELIM2                0x%08x\n" $val
	printf "              SPF_RATELIM           %u\n" $SPF_RATELIM

	val=`devmem $SPF_RATELIM3`
	SPF_RATELIM=$(($val & 0x000000ff))

	printf "        SPF_RATELIM3                0x%08x\n" $val
	printf "              SPF_RATELIM           %u\n" $SPF_RATELIM

	val=`devmem $SPF_CONST0`

	printf "        SPF_CONST0                  0x%08x\n" $val
	printf "              SPF_CONST             0x%08x\n" $val

	val=`devmem $SPF_CONST1`

	printf "        SPF_CONST1                  0x%08x\n" $val
	printf "              SPF_CONST             0x%08x\n" $val

	val=`devmem $SPF_CONST2`

	printf "        SPF_CONST2                  0x%08x\n" $val
	printf "              SPF_CONST             0x%08x\n" $val

	val=`devmem $SPF_CONST3`

	printf "        SPF_CONST3                  0x%08x\n" $val
	printf "              SPF_CONST             0x%08x\n" $val

	val=`devmem $SPF_CONST4`

	printf "        SPF_CONST4                  0x%08x\n" $val
	printf "              SPF_CONST             0x%08x\n" $val

	val=`devmem $SPF_CONST5`

	printf "        SPF_CONST5                  0x%08x\n" $val
	printf "              SPF_CONST             0x%08x\n" $val

	val=`devmem $SPF_CONST6`

	printf "        SPF_CONST6                  0x%08x\n" $val
	printf "              SPF_CONST             0x%08x\n" $val

	val=`devmem $SPF_CONST7`

	printf "        SPF_CONST7                  0x%08x\n" $val
	printf "              SPF_CONST             0x%08x\n" $val

	val=`devmem $SPF_INSTRW2`
	SPF_INSTR_W2=$(($val & 0x00003fff))

	printf "        SPF_INSTRW2                 0x%08x\n" $val
	printf "              SPF_INSTR_W2          0x%08x\n" $SPF_INSTR_W2

	val=`devmem $SPF_INSTRW1`

	printf "        SPF_INSTRW1                 0x%08x\n" $val
	printf "              SPF_INSTRW1           0x%08x\n" $val

	val=`devmem $SPF_INSTRW0`

	printf "        SPF_INSTRW0                 0x%08x\n" $val
	printf "              SPF_INSTRW1           0x%08x\n" $val

	val=`devmem $SPF_INSTR_CTL`
	SPF_INSTR_WEN=$((($val & 0x80000000) >> 31))
	SPF_INSTR_REN=$((($val & 0x40000000) >> 30))
	SPF_INSTR_PTR=$(($val & 0x0000003f))

	printf "        SPF_INSTR_CTL               0x%08x\n" $val
	printf "              SPF_INSTR_WEN         %u\n" $SPF_INSTR_WEN
	printf "              SPF_INSTR_REN         %u\n" $SPF_INSTR_REN
	printf "              SPF_INSTR_PTR         %u\n" $SPF_INSTR_PTR

	val=`devmem $SPF_LOG_BEGIN`

	printf "        SPF_LOG_BEGIN               0x%08x\n" $val
	printf "              SPF_LOG_BEGIN         0x%08x\n" $val
	
	val=`devmem $SPF_LOG_END`

	printf "        SPF_LOG_END                 0x%08x\n" $val
	printf "              SPF_LOG_END           0x%08x\n" $val
	
	val=`devmem $SPF_LOG_HWPTR`

	printf "        SPF_LOG_HWPTR               0x%08x\n" $val
	printf "              SPF_LOG_HWPTR         0x%08x\n" $val
	
	val=`devmem $SPF_LOG_SWPTR`

	printf "        SPF_LOG_SWPTR               0x%08x\n" $val
	printf "              SPF_LOG_SWPTR         0x%08x\n" $val
	
	val=`devmem $SPF_LOG_MAP0`
	SPF_LOGMAP3=$((($val & 0xff000000) >> 24))
	SPF_LOGMAP2=$((($val & 0x00ff0000) >> 16))
	SPF_LOGMAP1=$((($val & 0x0000ff00) >> 8))
	SPF_LOGMAP0=$(($val & 0x000000ff))

	printf "        SPF_LOG_MAP0                0x%08x\n" $val
	printf "              SPF_LOGMAP3           %u\n" $SPF_LOGMAP3
	printf "              SPF_LOGMAP2           %u\n" $SPF_LOGMAP2
	printf "              SPF_LOGMAP1           %u\n" $SPF_LOGMAP1
	printf "              SPF_LOGMAP0           %u\n" $SPF_LOGMAP0
	
	val=`devmem $SPF_LOG_MAP1`
	SPF_LOGMAP7=$((($val & 0xff000000) >> 24))
	SPF_LOGMAP6=$((($val & 0x00ff0000) >> 16))
	SPF_LOGMAP5=$((($val & 0x0000ff00) >> 8))
	SPF_LOGMAP4=$(($val & 0x000000ff))

	printf "        SPF_LOG_MAP1                0x%08x\n" $val
	printf "              SPF_LOGMAP7           %u\n" $SPF_LOGMAP7
	printf "              SPF_LOGMAP6           %u\n" $SPF_LOGMAP6
	printf "              SPF_LOGMAP5           %u\n" $SPF_LOGMAP5
	printf "              SPF_LOGMAP4           %u\n" $SPF_LOGMAP4
	
	val=`devmem $SPF_LOG_THRESH0`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH0             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_LOG_THRESH1`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH1             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_LOG_THRESH2`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH2             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_LOG_THRESH3`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH3             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_LOG_THRESH4`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH4             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_LOG_THRESH5`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH5             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_LOG_THRESH6`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH6             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_LOG_THRESH7`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH7             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_LOG_THRESH8`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_THRESH=$(($val & 0x0000ffff))

	printf "        SPF_LOG_THRESH8             0x%08x\n" $val
	printf "              SPF_COUNT             %u\n" $SPF_COUNT
	printf "              SPF_THRESH            %u\n" $SPF_THRESH
	
	val=`devmem $SPF_INTCNT`
	SPF_COUNT=$((($val & 0xffff0000) >> 16))
	SPF_INTCNT=$(($val & 0x0000001f))

	printf "        SPF_INTCNT                  0x%08x\n" $val
	printf "              SPF_INTCNT            %u\n" $SPF_INTCNT
	
	val=`devmem $SPF_INT_RAW`
	SPF_INT_RAW=$(($val & 0x00000001))

	printf "        SPF_INT_RAW                 0x%08x\n" $val
	printf "              SPF_INT_RAW           %u\n" $SPF_INT_RAW
	
	val=`devmem $SPF_INT_MASKED`
	SPF_INT_MASKED=$(($val & 0x00000001))

	printf "        SPF_INT_MASKED              0x%08x\n" $val
	printf "              SPF_INT_MASKED        %u\n" $SPF_INT_MASKED
	
	val=`devmem $SPF_MASK_SET`
	SPF_MASK_SET=$(($val & 0x00000001))

	printf "        SPF_MASK_SET                0x%08x\n" $val
	printf "              SPF_MASKSET           %u\n" $SPF_MASK_SET
	
	val=`devmem $SPF_MASK_CLR`
	SPF_MASK_CLR=$(($val & 0x00000001))

	printf "        SPF_MASK_CLR                0x%08x\n" $val
	printf "              SPF_MASKCLR           %u\n" $SPF_MASK_CLR
	
	printf "========================================================\n"
}

detect_variant

case $hw_variant in
j5e)
		setup_j5e
		;;
j6)
		setup_j6
		;;
*)
		echo "Invalid HW variant  $hw_variant."
		rSTAT=10
		exit $rSTAT
		;;
esac

if [ "$#" -lt 1 ]; then
	do_cpdma=1
	do_stateram=1
	do_stats=1
	do_ss=1
	do_port=1
	do_cpts=1
	do_ale=1
	do_sl=1
	do_mdio=1
	do_wr=1
	do_spf=1
else
	until [ -z "$1" ]
	do
		case $1 in
		cpdma)
			do_cpdma=1
			;;
		stateram)
			do_stateram=1
			;;
		stats)
			do_stats=1
			;;
		ss)
			do_ss=1
			;;
		port)
			do_port=1
			;;
		cpts)
			do_cpts=1
			;;
		ale)
			do_ale=1
			;;
		sl)
			do_sl=1
			;;
		mdio)
			do_mdio=1
			;;
		wr)
			do_wr=1
			;;
		spf)
			do_spf=1
			;;
		esac
		shift 1
	done
fi

if [ -n "$do_cpdma" ]; then
	dump_cpdma
fi
if [ -n "$do_stateram" ]; then
	dump_stateram_tx
	dump_stateram_rx
fi
if [ -n "$do_stats" ]; then
	dump_stats
fi
if [ -n "$do_ss" ]; then
	dump_ss
fi
if [ -n "$do_port" ]; then
	dump_port
fi
if [ -n "$do_cpts" ]; then
	dump_cpts
fi
if [ -n "$do_ale" ]; then
	dump_ale
fi
if [ -n "$do_sl" ]; then
	dump_sl "SL1" $SL1_BASE
	dump_sl "SL2" $SL2_BASE
fi
if [ -n "$do_mdio" ]; then
	dump_mdio
fi
if [ -n "$do_wr" ]; then
	dump_wr
fi
if [ -n "$do_spf" ]; then
	dump_spf "SPF1" $SPF1_BASE
	dump_spf "SPF2" $SPF2_BASE
fi
