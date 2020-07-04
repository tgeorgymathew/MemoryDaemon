char* stringToBinary(char* s)
{
    if(s == NULL) return 0; /* no input string */
    char *binary = malloc((strlen(s) * 8) + 1);
    strcpy(binary,"");
    char *ptr = s;
    int i;

    for(; *ptr != 0; ++ptr)
    {

        /* perform bitwise AND for every bit of the character */
        for(i = 7; i >= 0; --i){
            (*ptr & 1 << i) ? strcat(binary,"1") : strcat(binary,"0");
        }

    }

    return binary;
}

//void binaryToString(char *s)
//{
//    int i;
//    int binaryLength = strlen(binaryLength);
//    char *text;
//    for(i = 0; i < binaryLength; i+=8, s += 8)
//    {
//        char *byte = s;
//        byte[8] = '\0';
//        text++ = binaryToDecimal(byte, 8);
//    }
//    text -= symbolCount;
//}
