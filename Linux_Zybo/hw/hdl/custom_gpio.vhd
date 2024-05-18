----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 18.05.2022 11:40:17
-- Design Name: 
-- Module Name: custom_gpio - Dataflow
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity custom_gpio is
  Port ( 
    enable_i    : in    std_logic;
    write_i     : in    std_logic;
    gpio_io     : inout std_logic;
    read_o      : out   std_logic
  );
end custom_gpio;

architecture Dataflow of custom_gpio is

begin
    read_o <= gpio_io;
    with enable_i select
        gpio_io <= write_i when '1',
                    'Z'  when others;

end Dataflow;
