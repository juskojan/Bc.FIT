-- fsm.vhd: Finite State Machine
-- Author(s): 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (TEST1, TEST_A2, TEST_A3, TEST_A4, TEST_A5, TEST_A6, TEST_A7, TEST_A8, TEST_A9, TEST_A10, TEST_A_END,
					TEST_B2, TEST_B3, TEST_B4, TEST_B5, TEST_B6, TEST_B7, TEST_B8, TEST_B9, TEST_B10, TEST_B_END,
					PRINT_MESSAGE_OK, PRINT_MESSAGE_FAIL, FAIL, FINISH);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= TEST1;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when TEST1 =>
      next_state <= TEST1;
      if (KEY(1) = '1') then
        next_state <= TEST_A2;
      elsif (KEY(2) = '1') then
        next_state <= TEST_B2;       
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;
      
   -- - - - - - - - - - - - - - - - - - - - - - -
   -- FIRST CODE TEST
   -- - - - - - - - - - - - - - - - - - - - - - -
   
      when TEST_A2 =>
      next_state <= TEST_A2; 
      if (KEY(7) = '1') then
        next_state <= TEST_A3; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A3 =>
      next_state <= TEST_A3; 
      if (KEY(2) = '1') then
        next_state <= TEST_A4; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A4 =>
      next_state <= TEST_A4; 
      if (KEY(6) = '1') then
        next_state <= TEST_A5; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A5 =>
      next_state <= TEST_A5; 
      if (KEY(7) = '1') then
        next_state <= TEST_A6; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A6 =>
      next_state <= TEST_A6; 
      if (KEY(6) = '1') then
        next_state <= TEST_A7; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A7 =>
      next_state <= TEST_A7; 
      if (KEY(4) = '1') then
        next_state <= TEST_A8; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A8 =>
      next_state <= TEST_A8; 
      if (KEY(4) = '1') then
        next_state <= TEST_A9; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A9 =>
      next_state <= TEST_A9; 
      if (KEY(0) = '1') then
        next_state <= TEST_A10; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;        
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A10 =>
      next_state <= TEST_A10; 
      if (KEY(4) = '1') Then
        next_state <= TEST_A_END; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;   
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_A_END =>
      next_state <= TEST_A_END;
      if (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_OK; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;
      end if;
      
   -- - - - - - - - - - - - - - - - - - - - - - -
   -- SECOND CODE TEST
   -- - - - - - - - - - - - - - - - - - - - - - -
   
      when TEST_B2 =>
      next_state <= TEST_B2; 
      if (KEY(7) = '1') then
        next_state <= TEST_B3; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B3 =>
      next_state <= TEST_B3; 
      if (KEY(6) = '1') then
        next_state <= TEST_B4; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B4 =>
      next_state <= TEST_B4; 
      if (KEY(2) = '1') then
        next_state <= TEST_B5; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B5 =>
      next_state <= TEST_B5; 
      if (KEY(8) = '1') then
        next_state <= TEST_B6; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B6 =>
      next_state <= TEST_B6; 
      if (KEY(2) = '1') then
        next_state <= TEST_B7; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B7 =>
      next_state <= TEST_B7; 
      if (KEY(3) = '1') then
        next_state <= TEST_B8; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B8 =>
      next_state <= TEST_B8; 
      if (KEY(0) = '1') then
        next_state <= TEST_B9; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;      
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B9 =>
      next_state <= TEST_B9; 
      if (KEY(4) = '1') then
        next_state <= TEST_B10; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;        
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B10 =>
      next_state <= TEST_B10; 
      if (KEY(6) = '1') Then
        next_state <= TEST_B_END; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;      
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL; 
      end if;   
   -- - - - - - - - - - - - - - - - - - - - - - -
      when TEST_B_END =>
      next_state <= TEST_B_END;
      if (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_OK; 
      elsif (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;
      end if;
      
   -- - - - - - - - - - - - - - - - - - - - - - -
   -- OTHER STATES
   -- - - - - - - - - - - - - - - - - - - - - - -
   
   when FAIL =>
      next_state <= FAIL;
      if (KEY(14 downto 0) /= "000000000000000") then
        next_state <= FAIL;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_MESSAGE_FAIL;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_FAIL =>
      next_state <= PRINT_MESSAGE_FAIL;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_OK =>
      next_state <= PRINT_MESSAGE_OK;
      if (CNT_OF = '1') then
         next_state <= FINISH;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      next_state <= FINISH;
      if (KEY(15) = '1') then
         next_state <= TEST1; 
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= TEST1;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_OK =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_MX_MEM     <= '1';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_MESSAGE_FAIL =>
      FSM_CNT_CE     <= '1';
      FSM_MX_LCD     <= '1';
      FSM_MX_MEM     <= '0';
      FSM_LCD_WR     <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when FINISH =>
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR     <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR    <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

