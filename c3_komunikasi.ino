#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Struktur data untuk tabel periodik
struct Element {
  const char* index;
  const char* symbol;
  const char* name;
  int atomic_number;
  float atomic_mass;
};

// Tabel periodik (20 elemen pertama, bisa diperluas)
Element tabelPeriodik[] = {
  {"017" , "H" , "Hidrogen" , 1, 1.008},
  {"016" , "Li" , "Litium" , 3, 6.94},
  {"015" , "Na" , "Natrium" , 11, 22.99},
  {"014" , "K" , "Kalium" , 19, 39.098},
  {"013" , "Rb" , "Rubidium" , 37, 85.468},
  {"012" , "Cs" , "Sesium" , 55, 132.905},
  {"011" , "Fr" , "Fransium" , 87, 223},
  {"010" , "Be" , "Berilium" , 4, 9.0122},
  {"000" , "Mg" , "Magnesium" , 12, 24.305},
  {"001" , "Ca" , "Kalsium" , 20, 40.078},
  {"002" , "Sr" , "Stronsium" , 38, 87.62},
  {"003" , "Ba" , "Barium" , 56, 137.327},
  {"004" , "Ra" , "Radium" , 88, 226},
  {"005" , "Sc" , "Skandium" , 21, 44.956},
  {"006" , "Y" , "Yttrium" , 39, 88.906},
  {"007" , "-" , "-" , 0, 0.0},
  {"117" , "-" , "-" , 0, 0.0},
  {"116" , "Ti" , "Titanium" , 22, 47.867},
  {"115" , "Zr" , "Zirkonium" , 40, 91.224},
  {"114" , "Hf" , "Hafnium" , 72, 178.49},
  {"113" , "Rf" , "Rutherfordium" , 104, 267},
  {"112" , "V" , "Vanadium" , 23, 50.942},
  {"111" , "Nb" , "Niobium" , 41, 92.906},
  {"110" , "Ta" , "Tantalum" , 73, 180.948},
  {"100" , "Db" , "Dubnium" , 105, 268},
  {"101" , "Cr" , "Kromium" , 24, 51.996},
  {"102" , "Mo" , "Molibdenum" , 42, 95.95},
  {"103" , "W" , "Tungsten" , 74, 183.84},
  {"104" , "Sg" , "Seaborgium" , 106, 269},
  {"105" , "Mn" , "Mangan" , 25, 54.938},
  {"106" , "Tc" , "Teknesium" , 43, 98},
  {"107" , "Re" , "Renium" , 75, 186.207},
  {"217" , "Bh" , "Bohrium" , 107, 270},
  {"216" , "Fe" , "Besi" , 26, 55.845},
  {"215" , "Ru" , "Rutenium" , 44, 101.07},
  {"214" , "Os" , "Osmium" , 76, 190.23},
  {"213" , "Hs" , "Hassium" , 108, 277},
  {"212" , "Co" , "Kobalt" , 27, 58.933},
  {"211" , "Rh" , "Rodium" , 45, 102.905},
  {"210" , "Ir" , "Iridium" , 77, 192.217},
  {"200" , "Mt" , "Meitnerium" , 109, 278},
  {"201" , "Ni" , "Nikel" , 28, 58.693},
  {"202" , "Pd" , "Paladium" , 46, 106.42},
  {"203" , "Pt" , "Platina" , 78, 195.084},
  {"204" , "Ds" , "Darmstadtium" , 110, 281},
  {"205" , "Cu" , "Tembaga" , 29, 63.546},
  {"206" , "Ag" , "Perak" , 47, 107.868},
  {"207" , "Au" , "Emas" , 79, 196.967},
  {"317" , "Rg" , "Roentgenium" , 111, 282},
  {"316" , "Zn" , "Seng" , 30, 65.38},
  {"315" , "Cd" , "Kadmium" , 48, 112.414},
  {"314" , "Hg" , "Raksa" , 80, 200.592},
  {"313" , "Cn" , "Kopernisium" , 112, 285},
  {"312" , "B" , "Boron" , 5, 10.81},
  {"311" , "Al" , "Aluminium" , 13, 26.982},
  {"310" , "Ga" , "Galium" , 31, 69.723},
  {"300" , "In" , "Indium" , 49, 114.818},
  {"301" , "Tl" , "Talium" , 81, 204.383},
  {"302" , "Nh" , "Nihonium" , 113, 286},
  {"303" , "C" , "Karbon" , 6, 12.011},
  {"304" , "Si" , "Silikon" , 14, 28.085},
  {"305" , "Ge" , "Germanium" , 32, 72.63},
  {"306" , "Sn" , "Timah" , 50, 118.71},
  {"307" , "Pb" , "Timbal" , 82, 207.2},
  {"417" , "Fl" , "Flerovium" , 114, 289},
  {"416" , "N" , "Nitrogen" , 7, 14.007},
  {"415" , "P" , "Fosfor" , 15, 30.974},
  {"414" , "As" , "Arsen" , 33, 74.922},
  {"413" , "Sb" , "Antimon" , 51, 121.76},
  {"412" , "Bi" , "Bismut" , 83, 208.98},
  {"411" , "Mc" , "Moscovium" , 115, 288},
  {"410" , "O" , "Oksigen" , 8, 15.999},
  {"400" , "S" , "Belerang" , 16, 32.06},
  {"401" , "Se" , "Selenium" , 34, 78.971},
  {"402" , "Te" , "Tellurium" , 52, 127.6},
  {"403" , "Po" , "Polonium" , 84, 209},
  {"404" , "Lv" , "Livermorium" , 116, 293},
  {"405" , "F" , "Fluorin" , 9, 18.998},
  {"406" , "Cl" , "Klorin" , 17, 35.45},
  {"407" , "Br" , "Bromin" , 35, 79.904},
  {"517" , "I" , "Iodium" , 53, 126.9},
  {"516" , "At" , "Astatin" , 85, 210},
  {"515" , "Ts" , "Tennessine" , 117, 294},
  {"514" , "He" , "Helium" , 2, 4.0026},
  {"513" , "Ne" , "Neon" , 10, 20.18},
  {"512" , "Ar" , "Argon" , 18, 39.948},
  {"511" , "Kr" , "Kripton" , 36, 83.798},
  {"510" , "Xe" , "Xenon" , 54, 131.293},
  {"500" , "Rn" , "Radon" , 86, 222},
  {"501" , "Og" , "Oganesson" , 118, 294},

  {"616" , "Lu" , "Lutetium" , 71, 174.967},
  {"617" , "Lr" , "Lawrencium" , 103, 262},
  {"614" , "Yb" , "Ytterbium" , 70, 173.045},
  {"615" , "No" , "Nobelium" , 102, 259},
  {"612" , "Tm" , "Thulium" , 69, 168.934},
  {"613" , "Md" , "Mendelevium" , 101, 258},
  {"610" , "Er" , "Erbium" , 68, 167.259},
  {"611" , "Fm" , "Fermium" , 100, 257},
  {"601" , "Ho" , "Holmium" , 67, 164.93},
  {"600" , "Es" , "Einsteinium" , 99, 252},
  {"603" , "Dy" , "Dysprosium" , 66, 162.5},
  {"602" , "Cf" , "Californium" , 98, 251},
  {"605" , "Tb" , "Terbium" , 65, 158.925},
  {"604" , "Bk" , "Berkelium" , 97, 247},
  {"607" , "Gd" , "Gadolinium" , 64, 157.25},
  {"606" , "Cm" , "Curium" , 96, 247},
  {"716" , "Eu" , "Europium" , 63, 151.964},
  {"717" , "Am" , "Amerisium" , 95, 243},
  {"714" , "Sm" , "Samarium" , 62, 150.36},
  {"715" , "Pu" , "Plutonium" , 94, 244},
  {"712" , "Pm" , "Prometium" , 61, 145},
  {"713" , "Np" , "Neptunium" , 93, 237},
  {"710" , "Nd" , "Neodimium" , 60, 144.242},
  {"711" , "U" , "Uranium" , 92, 238.029},
  {"701" , "Pr" , "Praseodimium" , 59, 140.908},
  {"700" , "Pa" , "Protaktinium" , 91, 231.035},
  {"703" , "Ce" , "Cerium" , 58, 140.116},
  {"702" , "Th" , "Thorium" , 90, 232.037},
  {"705" , "La" , "Lantanum" , 57, 138.905},
  {"704" , "Ac" , "Aktinium" , 89, 227},
  
};

const int jumlahElemen = sizeof(tabelPeriodik) / sizeof(tabelPeriodik[0]);

void centerText(int row, const char* text) {
  int len = strlen(text);
  int start = (20 - len) / 2;     // posisi kolom mulai
  if (start < 0) start = 0;
  lcd.setCursor(start, row);
  lcd.print(text);
}


void setup() {
  Serial.begin(9600);       // ke PC
  Serial1.begin(115200, SERIAL_8N1, 20, 21); // RX=20, TX=21

  lcd.init();        // Inisialisasi LCD
  lcd.backlight();   // Nyalakan lampu LCD

  lcd.clear();
  centerText(0, "WELCOME TO");
  centerText(1, "TABEL PERIODIK");
  centerText(2, "BRAILE");
  centerText(3, ".....");
  
  Serial.println("=== ESP32-C3 Periodic Table Receiver ===");
  Serial.println("Menunggu nomor atom dari ESP32...");

}

void loop() {
  if (Serial1.available()) {
    // Baca string penuh (misal "003\n")
    String data = Serial1.readStringUntil('\n');
    data.trim(); // hapus karakter spasi atau newline
    
    Serial.printf("\nDiterima index: \"%s\"\n", data.c_str());

    bool ditemukan = false;
    for (int i = 0; i < jumlahElemen; i++) {
      // Bandingkan string index
      if (strcmp(data.c_str(), tabelPeriodik[i].index) == 0) {
        Serial.println("=== Data Unsur Ditemukan ===");
        Serial.printf("Index       : %s\n", tabelPeriodik[i].index);
        Serial.printf("Simbol      : %s\n", tabelPeriodik[i].symbol);
        Serial.printf("Nama        : %s\n", tabelPeriodik[i].name);
        Serial.printf("Nomor Atom  : %d\n", tabelPeriodik[i].atomic_number);
        Serial.printf("Massa Atom  : %.4f u\n", tabelPeriodik[i].atomic_mass);
        Serial.println("============================");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Nama  : ");
        lcd.print(tabelPeriodik[i].name);
        lcd.setCursor(0, 1);
        lcd.print("Simbol: ");
        lcd.print(tabelPeriodik[i].symbol);
        lcd.setCursor(0, 2);
        lcd.print("N.Atom: ");
        lcd.print(tabelPeriodik[i].atomic_number);
        lcd.setCursor(0, 3);
        lcd.print("M.Atom: ");
        lcd.print(tabelPeriodik[i].atomic_mass, 3);
        
        ditemukan = true;
        break;
      }
    }

    if (!ditemukan) {
      Serial.println("Index tidak ditemukan di tabel periodik ❌");
    }
  }
}
