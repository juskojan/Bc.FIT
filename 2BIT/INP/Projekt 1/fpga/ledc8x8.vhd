--vypracoval Ján Jusko 	xjusko00
--						2.11.2014

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
	port(
		SMCLK: in std_logic;
		RESET: in std_logic;
		ROW: out std_logic_vector (0 to 7);
		LED: out std_logic_vector (0 to 7)
	);
end ledc8x8;

architecture behavior of ledc8x8 is
	signal ce: std_logic; 								-- clock enable
	signal ce_cnt: std_logic_vector(7 downto 0) := (others => '0');		-- kontrolny citac
	signal row_cnt: std_logic_vector(7 downto 0);		-- citac riadku
	signal inicialy: std_logic_vector(7 downto 0);
	
begin
	
	--delenie signalu 256
	vydelSignal: process(RESET, SMCLK)
	begin
		if RESET = '1' then
			ce_cnt <= "00000000";
		elsif (SMCLK'event and SMCLK = '1') then
			ce_cnt <= ce_cnt + 1;
		end if;
	end process vydelSignal;
	
	ce <= '1' when ce_cnt = "10000000" else '0';
	
	
	--dekoder riadkov
	dekoder: process(row_cnt)
	begin
		case row_cnt is
			when "00000001" => inicialy <= "10011111";
			when "00000010" => inicialy <= "01101111";
			when "00000100" => inicialy <= "01101001";
			when "00001000" => inicialy <= "11100110";
			when "00010000" => inicialy <= "11100110";
			when "00100000" => inicialy <= "11101110";
			when "01000000" => inicialy <= "11111110";
			when "10000000" => inicialy <= "11111110";
			when others =>     inicialy <= "11111111";
		end case;
	end process dekoder;	
	
	
	--rotacia riadkov
	rotacia: process(RESET, ce)
	begin
		if RESET = '1' then
			row_cnt <= "00000001";
		elsif (ce'event and ce = '1') then
			case row_cnt is         
				when "00000001" => row_cnt <= "00000010";
				when "00000010" => row_cnt <= "00000100";
				when "00000100" => row_cnt <= "00001000";
				when "00001000" => row_cnt <= "00010000";
				when "00010000" => row_cnt <= "00100000";
				when "00100000" => row_cnt <= "01000000";
				when "01000000" => row_cnt <= "10000000";
				when "10000000" => row_cnt <= "00000001";	
				when others => null;
			end case;
		end if;	
	end process rotacia;		
	
	
	--svietenie
	sviet: process(inicialy)
	begin
		ROW <= row_cnt;
		LED <= inicialy;
	end process sviet;
	
end architecture behavior;
	
