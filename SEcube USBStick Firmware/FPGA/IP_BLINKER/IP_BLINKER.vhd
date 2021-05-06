--  ******************************************************************************
--  * File Name          : IP_BLINKER.vhd
--  * Description        : IP core on the SEcube™ FPGA which blinks LEDs according
--  *                      to the passed input
--  ******************************************************************************
--  *
--  * Copyright(c) 2016-present Blu5 Group <https://www.blu5group.com>
--  *
--  * This library is free software; you can redistribute it and/or
--  * modify it under the terms of the GNU Lesser General Public
--  * License as published by the Free Software Foundation; either
--  * version 3 of the License, or (at your option) any later version.
--  *
--  * This library is distributed in the hope that it will be useful,
--  * but WITHOUT ANY WARRANTY; without even the implied warranty of
--  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
--  * Lesser General Public License for more details.
--  *
--  * You should have received a copy of the GNU Lesser General Public
--  * License along with this library; if not, see <https://www.gnu.org/licenses/>.
--  *
--  ******************************************************************************

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity IP_BLINKER is
	port(	
			cpu_fpga_bus_a		: in std_logic_vector(5 downto 0);
			cpu_fpga_bus_d		: in std_logic_vector(15 downto 0);
			cpu_fpga_bus_noe    : in std_logic;	
			cpu_fpga_bus_nwe    : in std_logic;		
			cpu_fpga_bus_ne1    : in std_logic;		
			cpu_fpga_clk		: in std_logic;
			cpu_fpga_int_n      : out std_logic;
			cpu_fpga_rst		: in std_logic;
			fpga_io_gp			: out std_logic_vector(7 downto 0)
		);
end IP_BLINKER;

architecture BEHAVIORAL of IP_BLINKER is
	
	-- FSM STATES
	type input_detector_state_type is (OFF, IDLE, WAIT_ADDSET, WAIT_DATAST);
	signal STATE : input_detector_state_type;

	 
begin

	process(cpu_fpga_clk) 
	begin

		if(cpu_fpga_rst = '1') then
			STATE 			<= IDLE;
			cpu_fpga_int_n  <= '0';
			fpga_io_gp		<= (others => '1');

		elsif(rising_edge(cpu_fpga_clk)) then

			-- FSM behavior
			case STATE is
			
				when IDLE =>
					if(cpu_fpga_bus_ne1 = '0') then 
						STATE <= WAIT_ADDSET;
					end if;

				when WAIT_ADDSET => 
					if(cpu_fpga_bus_nwe = '0') then
						STATE <= WAIT_DATAST;	
					else
						STATE <= WAIT_ADDSET;
					end if;

				when WAIT_DATAST =>
					if(cpu_fpga_bus_nwe = '1') then
						fpga_io_gp	<= not(cpu_fpga_bus_d(7 downto 0));
						STATE 		<= IDLE;
					else
						STATE 	<= WAIT_DATAST;
					end if;

				when others =>
					STATE <= OFF;

			end case;
		end if;
	end process; 

end BEHAVIORAL;