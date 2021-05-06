##  ******************************************************************************
##  * File Name          : sim_FPGA.tcl
##  * Description        : TCL script to be run into ModelSim for simulation
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

vlib work

vcom -2008 -work work IP_BLINKER.vhd  
                                                                                                                   
vcom -2008 -work work TB_FPGA.vhd

vsim -t 100ps -voptargs=+acc work.TB_FPGA

# 	TOP
add wave -noupdate -group TOP TB_FPGA/hclk		
add wave -noupdate -group TOP TB_FPGA/fpga_clk 
add wave -noupdate -group TOP TB_FPGA/reset    
add wave -noupdate -group TOP TB_FPGA/data     
add wave -noupdate -group TOP TB_FPGA/address  
add wave -noupdate -group TOP TB_FPGA/noe		
add wave -noupdate -group TOP TB_FPGA/nwe		
add wave -noupdate -group TOP TB_FPGA/ne1		
add wave -noupdate -group TOP TB_FPGA/interrupt
add wave -noupdate -group TOP TB_FPGA/UUT/STATE
add wave -noupdate -group TOP TB_FPGA/leds

run 500 ns 

