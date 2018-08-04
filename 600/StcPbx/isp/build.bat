REM build ISP for STC89C516RD+

a51 isp.asm
bl51 isp.obj TO isp.abs

oh51 isp.abs HEXFILE (isp.hex)

