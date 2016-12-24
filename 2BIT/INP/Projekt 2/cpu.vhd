-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2014 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): 		  Ján Jusko xjusko00
--




library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;




-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru


   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti


   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data


   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is


-- PROGRAM COUNTER 13b
signal PC_reg     : std_logic_vector(12 downto 0);
signal PC_inc     : std_logic;
signal PC_dec     : std_logic;


-- MEMORY POINTER 13b
signal PTR_reg     : std_logic_vector(12 downto 0);
signal PTR_inc     : std_logic;
signal PTR_dec     : std_logic;


-- LOOP COUNTER 8b
signal CNT_reg     : std_logic_vector(7 downto 0);
signal CNT_inc     : std_logic;
signal CNT_dec     : std_logic;


-- MULTIPLEXORY 
signal sel : std_logic := '0';
signal sel2 : std_logic_vector(1 downto 0) := "01";


-- INSTRUKCIE
type inst is (incPTR, decPTR, incVALUE, decVALUE, whileS, whileE, putchar, readchar, halt, skip);


signal decoded_inst : inst;


-- STAVY MOOROVA FSM AUTOMATU
type FSM_state is (SIDLE, SFETCH, SDECODE, SHALT, SSKIP,
 SINCPTR, SDECPTR,             -- >/<
 SINCVALUE, SINCVALUE2, SDECVALUE, SDECVALUE2, -- +/-
 SPUTCHAR, SPUTCHAR2,           -- .
 SREADCHAR, SREADCHAR2,       -- ,
 SWHILES,SWHILES2,SWHILES3,SWHILES4,SWHILES5, -- [
 SWHILEE,SWHILEE1,SWHILEE2,SWHILEE3,SWHILEE4 -- ]
 );


signal Present_state: FSM_state;
signal Next_state : FSM_state;




begin
-----------------------Program Counter-----------------------
-------------------------------------------------------------
PC: process (CLK,RESET)
begin
        if (RESET = '1') then
                PC_reg <= (others => '0');
        elsif (CLK'event) and (CLK = '1') then
                if (PC_inc = '1') then
                        PC_reg <= PC_reg + "0000000000001";
                end if;
                if (PC_dec = '1') then
                        PC_reg <= PC_reg - "0000000000001";
                end if;
        end if;
end process PC;


-----------------------Memory pointer------------------------
-------------------------------------------------------------
PTR: process (CLK,RESET)
begin
        if (RESET = '1') then
                PTR_reg <= "1000000000000";
        elsif (CLK'event) and (CLK = '1') then
                if (PTR_inc = '1') then
                        if (PTR_reg = "1111111111111") then -- KRUHOVOST OVERFLOW
                                PTR_reg <= "1000000000000";
                        else
                                PTR_reg <= PTR_reg + "0000000000001";
                        end if;
                end if;
                if (PTR_dec = '1') then
                        if (PTR_reg = "1000000000000") then -- KRUHOVOST UNDERFLOW
                                PTR_reg <= "1111111111111";
                        else
                                PTR_reg <= PTR_reg - "0000000000001";
                        end if;
                end if;
        end if;
end process PTR;


-----------------------Loop counter--------------------------
-------------------------------------------------------------
CNT: process (CLK, RESET)
begin
        if (RESET = '1') then
                CNT_reg <= (others=>'0');
        elsif (CLK'event) and (CLK = '1') then
                if (CNT_inc = '1') then
                        CNT_reg <= CNT_reg + "00000001";
                end if;
                if (CNT_dec = '1') then
                        CNT_reg <= CNT_reg - "00000001";
                end if;
        end if;
end process CNT;


--------------------Finite state machine---------------------
-------------------------------------------------------------
FSM_pstate: process (CLK, RESET, EN)
begin
        if (RESET = '1') then
                Present_state <= SIDLE;
        elsif (CLK'event) and (CLK = '1') then
                if (EN = '1') then
                        Present_state <= Next_state;
                end if;
        end if;
end process FSM_pstate;


-----------------MULTIPLEXOR PC vs. PTR----------------------
-------------------------------------------------------------
MUX1: process (sel, PTR_reg, PC_reg) 
begin
        case sel is
                when '1' =>
                        DATA_ADDR <= PTR_reg;
                when '0' =>
                        DATA_ADDR <= PC_reg;
                when others =>
        end case;
end process MUX1;


-----------------MULTIPLEXOR WRITE DATA----------------------
-------------------------------------------------------------
MUX2: process (sel2, DATA_RDATA, IN_DATA)
begin
        case sel2 is
                when "01" =>
                        DATA_WDATA <= DATA_RDATA + "00000001";
                when "10" =>
                        DATA_WDATA <= DATA_RDATA - "00000001";
                when "11" =>
                        DATA_WDATA <= IN_DATA;
                when others =>
        end case;
end process MUX2;


--------------------Instruction decoder----------------------
-------------------------------------------------------------
INST_decoder: process (DATA_RDATA)
begin
case DATA_RDATA is
when X"00" =>
decoded_inst <= halt;
when X"3E" =>
decoded_inst <= incPTR;
when X"3C" =>
decoded_inst <= decPTR;
when X"2B" =>
decoded_inst <= incVALUE;
when X"2D" =>
decoded_inst <= decVALUE;
when X"5B" =>
decoded_inst <= whileS;
when X"5D" =>
decoded_inst <= whileE;
when X"2E" =>
decoded_inst <= putchar;
when X"2C" =>
decoded_inst <= readchar;
when others=>
decoded_inst <= skip;
end case;
end process INST_decoder;




--------------------FSM PROCESSOR----------------------------
-------------------------------------------------------------
FSMPROCESSOR: process (Present_state, decoded_inst, OUT_BUSY, DATA_RDATA, IN_VLD, CNT_reg)
begin
IN_REQ <='0';
OUT_WE<='0';
DATA_RDWR<='0';
DATA_EN<='0';
PC_inc<='0';
PC_dec<='0';
PTR_inc<='0';
PTR_dec<='0';
CNT_inc<='0';
CNT_dec<='0';
sel <= '0';
sel2 <= "01";
OUT_DATA <= (others => '0');

case Present_state is


when SIDLE => -- zaciatok, init, pri resete
Next_state <= SFETCH;


when SHALT => -- koniec programu
Next_state <= SHALT;


when SSKIP => -- preskoc komentare
PC_inc <= '1';
Next_state <= SFETCH;


when SFETCH => -- nacitaj instrukciu
DATA_EN <= '1';
DATA_RDWR <= '0';
sel <= '0';
Next_state <= SDECODE;




when SDECODE => -- dekoduj instrukciu
case decoded_inst is
when halt =>
Next_state <= SHALT;
when incPTR =>
Next_state <= SINCPTR;
when decPTR =>
Next_state <= SDECPTR;
when incVALUE =>
Next_state <= SINCVALUE;
when decVALUE =>
Next_state <= SDECVALUE;
when whileS =>
Next_state <= SWHILES;
when whileE =>
Next_state <= SWHILEE;
when putchar =>
Next_state <= SPUTCHAR;
when readchar =>
Next_state <= SREADCHAR;
when skip =>
Next_state <= SSKIP;

end case;


-- PTR inkrement (>)
when SINCPTR =>
PC_inc <= '1';
PTR_inc <= '1';
Next_state <= SFETCH;


-- PTR dekrement (<)
when SDECPTR =>
PC_inc <= '1';
PTR_dec <= '1';
Next_state <= SFETCH;


-- *PTR inkrement (+)
when SINCVALUE =>
sel <= '1';
DATA_RDWR <= '0';
DATA_EN <= '1'; 
Next_state <= SINCVALUE2;


when SINCVALUE2 =>
DATA_RDWR <= '1';
DATA_EN <= '1'; 
sel <= '1';
sel2 <= "01";
PC_inc <= '1';
Next_state <= SFETCH;


-- *PTR dekrement (-)
when SDECVALUE =>
sel <= '1';
DATA_RDWR <= '0';
DATA_EN <= '1';
Next_state <= SDECVALUE2;


when SDECVALUE2 =>
DATA_RDWR <= '1';
DATA_EN <= '1';
sel <= '1';
sel2 <= "10";
PC_inc <= '1';
Next_state <= SFETCH;



-- vypis znak (.)
when SPUTCHAR =>
DATA_RDWR <= '0';
DATA_EN <= '1';
sel <= '1';
Next_state <= SPUTCHAR2;


when SPUTCHAR2 =>
if (OUT_BUSY = '0') then
OUT_WE <= '1';
OUT_DATA <= DATA_RDATA;
PC_inc <= '1';
Next_state <= SFETCH;
else
Next_state <= SPUTCHAR2;
end if;


-- nacitaj znak (,)
when SREADCHAR =>
IN_REQ <= '1';
Next_state <= SREADCHAR2;


when SREADCHAR2 =>
if (IN_VLD = '1') then
sel <= '1';
sel2 <= "11";
DATA_RDWR <= '1';
DATA_EN <= '1';
PC_inc <= '1';
Next_state <= SFETCH;
else
IN_REQ <= '1';
Next_state <= SREADCHAR2;  
end if;



-- WHILE ZACIATOK --
when SWHILES =>
DATA_RDWR <= '0';
DATA_EN <= '1';
PC_inc <= '1';
sel <= '1';
Next_state <= SWHILES2;


when SWHILES2 =>
if (DATA_RDATA = "00000000") then
CNT_inc <= '1';
Next_state <= SWHILES3;
else
Next_state <= SFETCH;
end if;


when SWHILES3 =>
DATA_RDWR <= '0';
DATA_EN <= '1';
sel <= '0';
Next_state <= SWHILES4;


when SWHILES4 =>
if (decoded_inst = whileE) then
CNT_dec <= '1';
elsif (decoded_inst = whileS) then
CNT_inc <= '1';
end if;
PC_inc <= '1';
Next_state <= SWHILES5;


when SWHILES5 =>
if (CNT_reg = "00000000") then
Next_state <= SFETCH;
else
Next_state <= SWHILES3;
end if;



-- WHILE KONIEC --
when SWHILEE =>
DATA_RDWR <= '0';
DATA_EN <= '1';
sel <= '1';
Next_state <= SWHILEE1;


when SWHILEE1 =>
if (DATA_RDATA = "00000000") then
PC_inc <= '1';
Next_state <= SFETCH; 
else
CNT_inc <= '1';
PC_dec <= '1';
Next_state <= SWHILEE2;
end if;


when SWHILEE2 =>
sel <= '0';
DATA_RDWR <= '0';
DATA_EN <= '1';
Next_state <= SWHILEE3;


when SWHILEE3 =>
if (decoded_inst = whileS) then
CNT_dec <= '1';
elsif (decoded_inst = whileE) then
CNT_inc <= '1';
end if;
Next_state <= SWHILEE4;


when SWHILEE4 =>
if (CNT_reg = "00000000") then
PC_inc <= '1';
Next_state <= SFETCH;
else
PC_dec <= '1';
Next_state <= SWHILEE2;
end if;
end case;
end process FSMPROCESSOR;


end behavioral;

