library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity hough_transform is
    port (
        clk     : in std_logic;
        reset   : in std_logic;
        de_in   : in  std_logic;
        data_in : in std_logic_vector(23 downto 0);
        data_out: out std_logic_vector(23 downto 0);
    );
end hough_transform;

architecture rtl of hough_transform is

    signal pixel_hab          : std_logic;

begin

    --======= determine if is in the correct range
    process
        variable contx : integer range 0 to 1280;
        variable conty : integer range 0 to 721;
    begin
        wait until rising_edge(clk);
        if(reset='1')then
            contx := 0;
            conty := 0;
        else
        
        if(de_in='1')then
            contx := contx + 1;
        end if;
        
        if(contx=1279)then
            conty := conty + 1;
            contx := 0;
            if(conty = 721)then
            conty := 0;
            end if;        
        end if;
        

        end if;

        
        if(conty>480 && conty<710)then
            if(contx>160 && contx<1120)then
                pixel_hab <= '1';
            else
                pixel_hab <= '0';
            end if;
        else
            pixel_hab <= '0';
        end if;

    end process;



end architecture;