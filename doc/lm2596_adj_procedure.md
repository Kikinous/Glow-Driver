# LM2595-ADJ

# Given
Vout = 1.2V
Vin = 9.9V
Iload = 2.5A (Rload = 0.5 Ohm)


#Programming output voltage
Vout = Vref ( 1+R2/R1) Vref = 1.23V

R2 = R1 (Vout/Vref -1) avec 1k < R1 < 5k
pot : R2 + R1 = 10k
R1 = 8.2k
R2 = 1.8k

# induductor selection
* E.t
E.t = (Vin-Vout-Vsat) * (Vout+Vd)/(Vin-Vsat+Vd) * 1000 / 150kHz  (V.us)
Vsat = 1.16V
Vd   = 0.5V
E.t = (9.9-1.2-0.5) * (1.2+0.5)/(9.9-1.16+0.5) * 1000/150
    = 10.06 (V.us)
* figure 24 : 
L = 15uH et inductor code = L34
brand
Schott / hole    = 67148400   =
Schott / SMT     = 67148790   =
Pulse eng / hole = PE-53934   =
Pulse eng / SMT  = PE-53934-S = digikey# 553-1417-ND = 2.17
 
# output capacitor
* Table 2
  Vout ~ 2V --> 330uF/6.3V AVX TPS Series       = digikey 478-9193-1-ND = 1.8 EUR
                470uF/4V   Sprague 595D Series  = no stock

# Catch diode
SK34 - 4->6 Amp  = digikey SK34A-TPCT-ND = 0.5 EUR

#Input Capacitor (CIN)
1000uF/16V                      = digikey P15087CT-ND = 0.9 EUR


