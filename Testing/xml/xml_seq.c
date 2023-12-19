
#include "txml.h"



int main()
{
    tXML* xml;

    xml = xml_parse_seq(NULL, "<A aaa=\"YYY\"><B>");
    xml = xml_parse_seq(xml->altp, "<Z zzz=\"99 9\"/><M>MM</M></B><C><M><X>XX</X><YY yyy=\"888\">XX");
    xml = xml_parse_seq(xml->altp, "YY</YY>MM</M><C bbb=\"777\" xxx=\"000i\" xxxx=\"000\" >");
    xml = xml_parse_seq(xml->altp, "<X></X><M><X></X>YY<Y>oooo");
    xml = xml_parse_seq(xml->altp, "</Y></M></C></C></A>");
    //xml = xml_parse_seq(xml->altp, "<M></M><C></C>");
    xml = xml_parse_seq(xml->altp, "<M>");

    //close_xml(xml);

    if (xml->state<0) {
        print_message("パースエラー\n");
    }
    print_xml_tree(stderr, xml, "      ");
    print_message("\n----> %d\n", xml->state);

    Buffer buf = xml_inverse_parse(xml, 2);
    printf("%s\n", buf.buf);

    return 0;
}

