#include "common.h"

using namespace boost::spirit;

/* ------------------------------------------------------------- */
x3::rule<struct t42> rule42;
x3::rule<struct t9> rule9;
x3::rule<struct t10> rule10;
x3::rule<struct t1> rule1;
x3::rule<struct t5> rule5;
x3::rule<struct t19> rule19;
x3::rule<struct t12> rule12;
x3::rule<struct t16> rule16;
x3::rule<struct t31> rule31;
x3::rule<struct t6> rule6;
x3::rule<struct t2> rule2;
x3::rule<struct t0> rule0;
x3::rule<struct t13> rule13;
x3::rule<struct t15> rule15;
x3::rule<struct t17> rule17;
x3::rule<struct t23> rule23;
x3::rule<struct t28> rule28;
x3::rule<struct t4> rule4;
x3::rule<struct t20> rule20;
x3::rule<struct t3> rule3;
x3::rule<struct t27> rule27;
x3::rule<struct t14> rule14;
x3::rule<struct t21> rule21;
x3::rule<struct t25> rule25;
x3::rule<struct t22> rule22;
x3::rule<struct t26> rule26;
x3::rule<struct t18> rule18;
x3::rule<struct t7> rule7;
x3::rule<struct t24> rule24;
x3::rule<struct t8> rule8;
x3::rule<struct t11> rule11;
auto rule42_def = ( rule9 >> rule14 ) | ( rule10 >> rule1 );
BOOST_SPIRIT_DEFINE(rule42)
auto rule9_def = ( rule14 >> rule27 ) | ( rule1 >> rule26 );
BOOST_SPIRIT_DEFINE(rule9)
auto rule10_def = ( rule23 >> rule14 ) | ( rule28 >> rule1 );
BOOST_SPIRIT_DEFINE(rule10)
auto rule1_def = x3::char_("a");
BOOST_SPIRIT_DEFINE(rule1)
auto rule5_def = ( rule1 >> rule14 ) | ( rule15 >> rule1 );
BOOST_SPIRIT_DEFINE(rule5)
auto rule19_def = ( rule14 >> rule1 ) | ( rule14 >> rule14 );
BOOST_SPIRIT_DEFINE(rule19)
auto rule12_def = ( rule24 >> rule14 ) | ( rule19 >> rule1 );
BOOST_SPIRIT_DEFINE(rule12)
auto rule16_def = ( rule15 >> rule1 ) | ( rule14 >> rule14 );
BOOST_SPIRIT_DEFINE(rule16)
auto rule31_def = ( rule14 >> rule17 ) | ( rule1 >> rule13 );
BOOST_SPIRIT_DEFINE(rule31)
auto rule6_def = ( rule14 >> rule14 ) | ( rule1 >> rule14 );
BOOST_SPIRIT_DEFINE(rule6)
auto rule2_def = ( rule1 >> rule24 ) | ( rule14 >> rule4 );
BOOST_SPIRIT_DEFINE(rule2)
auto rule0_def = ( rule8 >> rule11 );
BOOST_SPIRIT_DEFINE(rule0)
auto rule13_def = ( rule14 >> rule3 ) | ( rule1 >> rule12 );
BOOST_SPIRIT_DEFINE(rule13)
auto rule15_def = ( rule1 ) | ( rule14 );
BOOST_SPIRIT_DEFINE(rule15)
auto rule17_def = ( rule14 >> rule2 ) | ( rule1 >> rule7 );
BOOST_SPIRIT_DEFINE(rule17)
auto rule23_def = ( rule25 >> rule1 ) | ( rule22 >> rule14 );
BOOST_SPIRIT_DEFINE(rule23)
auto rule28_def = ( rule16 >> rule1 );
BOOST_SPIRIT_DEFINE(rule28)
auto rule4_def = ( rule1 >> rule1 );
BOOST_SPIRIT_DEFINE(rule4)
auto rule20_def = ( rule14 >> rule14 ) | ( rule1 >> rule15 );
BOOST_SPIRIT_DEFINE(rule20)
auto rule3_def = ( rule5 >> rule14 ) | ( rule16 >> rule1 );
BOOST_SPIRIT_DEFINE(rule3)
auto rule27_def = ( rule1 >> rule6 ) | ( rule14 >> rule18 );
BOOST_SPIRIT_DEFINE(rule27)
auto rule14_def = x3::char_("b");
BOOST_SPIRIT_DEFINE(rule14)
auto rule21_def = ( rule14 >> rule1 ) | ( rule1 >> rule14 );
BOOST_SPIRIT_DEFINE(rule21)
auto rule25_def = ( rule1 >> rule1 ) | ( rule1 >> rule14 );
BOOST_SPIRIT_DEFINE(rule25)
auto rule22_def = ( rule14 >> rule14 );
BOOST_SPIRIT_DEFINE(rule22)
auto rule26_def = ( rule14 >> rule22 ) | ( rule1 >> rule20 );
BOOST_SPIRIT_DEFINE(rule26)
auto rule18_def = ( rule15 >> rule15 );
BOOST_SPIRIT_DEFINE(rule18)
auto rule7_def = ( rule14 >> rule5 ) | ( rule1 >> rule21 );
BOOST_SPIRIT_DEFINE(rule7)
auto rule24_def = ( rule14 >> rule1 );
BOOST_SPIRIT_DEFINE(rule24)
auto rule8_def = ( rule42 ) | ( rule42 >> rule8 );
BOOST_SPIRIT_DEFINE(rule8)
auto rule11_def = ( rule42 >> rule31 ) | ( rule42 >> rule11 >> rule31 );
BOOST_SPIRIT_DEFINE(rule11)

/* ------------------------------------------------------------- */

bool f1(const std::string &input)
{
    auto it = input.begin();
    auto res = x3::parse(it, input.end(), rule0);
    return res && it == input.end();
}