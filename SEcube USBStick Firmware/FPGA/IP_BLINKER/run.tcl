##  ******************************************************************************
##  * File Name          : run.tcl
##  * Description        : TCL script to be run into Diamond to synthesize
##  *					   and generate the correct se3_fpga_bitstream.h file
##  ******************************************************************************
##  *
##  * Copyright(c) 2016-present Blu5 Group <https://www.blu5group.com>
##  *
##  * This library is free software; you can redistribute it and/or
##  * modify it under the terms of the GNU Lesser General Public
##  * License as published by the Free Software Foundation; either
##  * version 3 of the License, or (at your option) any later version.
##  *
##  * This library is distributed in the hope that it will be useful,
##  * but WITHOUT ANY WARRANTY; without even the implied warranty of
##  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
##  * Lesser General Public License for more details.
##  *
##  * You should have received a copy of the GNU Lesser General Public
##  * License along with this library; if not, see <https://www.gnu.org/licenses/>.
##  *
##  ******************************************************************************

set platform $tcl_platform(platform)
set proj_path [pwd]
set open_bracket "{"
set flag 0

set exe_path [info nameofexecutable]
set end [string last "/" $exe_path]
set deploy_path [string range $exe_path 0 $end]
append deploy_path "ddtcmd"

# retrieve project and implementation names
set full_name [glob *.ldf]
set length [string length $full_name]
set begin 0
set end [expr $length - 5]
set proj_name [string range $full_name $begin $end]
set proj_file $proj_name
append proj_file ".ldf"

set inputfile [open $proj_file]

# $input will contain the contents of the file
set input [read $inputfile]

# $lines will be an array containing each line of test.txt
set lines [split $input "\n"]

# Loop through each line
foreach line $lines {
   set found [string match "*default_implementation*" $line]
   if {$found > 0} {
      set begin [string first "default_implementation" $line]
      set begin [expr $begin + 24]
      set end [string length $line]
      set end [expr $end - 3]
      set impl_name [string range $line $begin $end]
   }
}

# Close
close $inputfile

set impl_path $proj_path
append impl_path "/" $impl_name

set xcf_path $impl_path
append xcf_path "/" $impl_name ".xcf"

# RUN CONFIG #

prj_run Map -impl $impl_name -task MapTrace
prj_run PAR -impl $impl_name -task PARTrace
prj_run PAR -impl $impl_name -task IOTiming
prj_run Export -impl $impl_name -task Bitgen
prj_run Export -impl $impl_name -task Jedecgen

# ########## .LDF FILE ########## #

set jed_path [pwd]
append jed_path "/"
append jed_path $impl_name "/"
append jed_path $proj_name "_" $impl_name ".jed"

set inputfile [open $jed_path]

# $input will contain the contents of the file
set input [read $inputfile]

# $lines will be an array containing each line of test.txt
set lines [split $input "\n"]

# Loop through each line
foreach line $lines {
	set len [string length $line]
	if {$len == 6} {
		set first [string index $line 0]
		if {$first == "C"} {
			set checksum [string range $line 1 4]
		}
	}
}

# Close
close $inputfile


# ########## .XCF FILE ########## #

set line1 "<File>"
append line1 $jed_path
append line1 "</File>"

set systime [clock seconds]
set now [clock format $systime -format %m]
append now "/" [clock format $systime -format %d]
append now "/" [clock format $systime -format %y]
append now " " [clock format $systime -format %T]

set line2 "<FileTime>"
append line2 $now
append line2 "</FileTime>"

set line3 "<JedecChecksum>"
append line3 "0x"
append line3 $checksum
append line3 "</JedecChecksum>"

set outputFile [open $xcf_path w]

# Write first part of se3_fpga_bitstream.h file
puts $outputFile "<?xml version='1.0' encoding='utf-8' ?>
<!DOCTYPE		ispXCF	SYSTEM	\"IspXCF.dtd\" >
<ispXCF version=\"3.10.0\">
	<Comment></Comment>
	<Chain>
		<Comm>JTAG</Comm>
		<Device>
			<SelectedProg value=\"TRUE\"/>
			<Pos>1</Pos>
			<Vendor>Lattice</Vendor>
			<Family>MachXO2</Family>
			<Name>LCMXO2-7000HE</Name>
			<IDCode>0x012b5043</IDCode>
			<Package>All</Package>
			<PON>LCMXO2-7000HE</PON>
			<Bypass>
				<InstrLen>8</InstrLen>
				<InstrVal>11111111</InstrVal>
				<BScanLen>1</BScanLen>
				<BScanVal>0</BScanVal>
			</Bypass>"
puts $outputFile $line1
puts $outputFile $line2
puts $outputFile $line3
puts $outputFile "<Operation>FLASH Erase,Program,Verify</Operation>
			<Option>
				<SVFVendor>JTAG STANDARD</SVFVendor>
				<IOState>HighZ</IOState>
				<PreloadLength>664</PreloadLength>
				<IOVectorData>0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF</IOVectorData>
				<SVFProcessor>SVF Processor</SVFProcessor>
				<Usercode>0x00000000</Usercode>
				<AccessMode>FLASH</AccessMode>
			</Option>
		</Device>
	</Chain>
	<ProjectOptions>
		<Program>SEQUENTIAL</Program>
		<Process>ENTIRED CHAIN</Process>
		<OperationOverride>No Override</OperationOverride>
		<StartTAP>TLR</StartTAP>
		<EndTAP>TLR</EndTAP>
		<VerifyUsercode value=\"FALSE\"/>
		<TCKDelay>1</TCKDelay>
	</ProjectOptions>
	<CableOptions>
		<CableName>USB</CableName>
		<PortAdd>EzUSB-0</PortAdd>
		<PROGConnect value=\"TRUE\"/>
		<INITConnect value=\"TRUE\"/>
	</CableOptions>
</ispXCF>"

# Close
close $outputFile


# add xcf file to project

#pgr_project close
#prj_src add -exclude $xcf_path
#prj_src enable $xcf_path
#pgr_project open $xcf_path

set synthesis_dir $impl_path
append synthesis_dir "/synthesis"
file mkdir $synthesis_dir

set algo_file $synthesis_dir
append algo_file "/" $impl_name "_algo.vme"

set data_file $synthesis_dir
append data_file "/" $impl_name "_data.vme"


# DEPLOYMENT TOOL

exec $deploy_path -oft -slimvme -if $xcf_path -hex -ofa $algo_file -ofd $data_file


# ##############################################


set SE3_FPGA_BITSTREAM_H $synthesis_dir
append SE3_FPGA_BITSTREAM_H "/" "se3_fpga_bitstream.h"
# Open se3_fpga_bitstream.h for writing
set outputFile [open $SE3_FPGA_BITSTREAM_H w]

# Write first part of se3_fpga_bitstream.h file
puts $outputFile "#ifndef SE3_FPGA_BITSTREAM_H_\n#define SE3_FPGA_BITSTREAM_H_\n\n#include <stdint.h>\n\n\nconst uint8_t __fpga_alg\[\] = {"

#************************ *_algo.c FILE ****************************
# Open *_algo.c for reading
# $file will contain the file pointer to proj_name_algo.c (file must exist)
set algoFile $synthesis_dir
append algoFile "/" $impl_name
append algoFile "_algo.c"

set inputfile [open $algoFile]

# $input will contain the contents of the file
set input [read $inputfile]

# $lines will be an array containing each line of test.txt
set lines [split $input "\n"]

# Loop through each line
foreach line $lines {
	if {$flag == 0} {
		set test [string first $open_bracket $line]
		if {$test > 0} {
			set flag 1
		}
		set test2 [string match "*AlgoSize*" $line]
		if {$test2 > 0} {
			set length [string length $line]
			set begin [expr $length - 7]
			set end [expr $length - 2]
			set AlgoSize [string range $line $begin $end]
		}
	} else {
		puts $outputFile $line
	}
}
set flag 0

# Close
close $inputfile


# Write second part of se3_fpga_bitstream.h file
puts $outputFile "\n const uint8_t __fpga_data\[\] = {"

#************************ _data.c FILE ****************************
# Open *_data.c for reading
# $file will contain the file pointer to proj_name_data.c (file must exist)
set dataFile $synthesis_dir
append dataFile "/" $impl_name
append dataFile "_data.c"

set inputfile [open $dataFile]

# $input will contain the contents of the file
set input [read $inputfile]

# $lines will be an array containing each line of test.txt
set lines [split $input "\n"]

# Loop through each line
foreach line $lines {
	if {$flag == 0} {
		set test [string first $open_bracket $line]
		if {$test > 0} {
			set flag 1
		}
		set test2 [string match "*DataSize*" $line]
		if {$test2 > 0} {
			set length [string length $line]
			set begin [expr $length - 7]
			set end [expr $length - 2]
			set DataSize [string range $line $begin $end]
		}
	} else {
		puts $outputFile $line
	}
}
set flag 0

# Close
close $inputfile

# Append dimensions of _algo and _data vectors in se3_fpga_bitstream.h file
set algoline "\n\n\nuint32_t g_iAlgoSize = "
append algoline $AlgoSize
append algoline ";"
puts $outputFile $algoline

set dataline "uint32_t g_iDataSize = "
append dataline $DataSize
append dataline ";"
puts $outputFile $dataline

set dataline "#endif /* SE3_FPGA_BITSTREAM_H_ */"
puts $outputFile $dataline

# Close se3_fpga_bitstream.h file
close $outputFile

