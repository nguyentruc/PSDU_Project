#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "sms_pdu.h"

static const char *hexchar = "0123456789ABCDEF";

static inline char hex2c(uint8_t d)
{
    return hexchar[d];
}

/*
 * http://twit88.com/home/utility/sms-pdu-encode-decode
 */
//static int decode7(uint8_t *in, size_t in_sz, uint8_t *out, size_t out_sz)
//{
//    size_t out_len_max = (in_sz * 8) / 7;
//    int state = 0;
//
//    if ( out_len_max > out_sz ) {
//        return -1;
//    }
//
//    uint8_t prev = 0;
//    for ( int i = 0, j = 0, shift = 8; (i < in_sz) && (j < in_sz); ++i, ++j, --shift ) {
//
//        if ( shift == 1 )
//		{
//			prev >>= 1;
//		    switch (state)
//		    {
//		    case 0:
//		    	if (prev == 27)
//		    	{
//		    		state = 1;
//		    	}
//		    	else out[j] = prev;
//		    	break;
//		    case 1:
//		    	state = 0;
//		    	switch (prev)
//		    	{
//		        case    10:
//		            out[j]=12;
//		            break;
//		        case    20:
//		            out[j]='^';
//		            break;
//		        case    40:
//		            out[j]='{';
//		            break;
//		        case    41:
//		            out[j]='}';
//		            break;
//		        case    47:
//		            out[j]='\\';
//		            break;
//		        case    60:
//		            out[j]='[';
//		            break;
//		        case    61:
//		            out[j]='~';
//		            break;
//		        case    62:
//		            out[j]=']';
//		            break;
//		       case    64:
//		            out[j]='|';
//		            break;
//		        default:
//		            out[j]='?';
//		            break;
//		    	}
//		    	break;
//		    }
//            ++j;
//            shift = 8;
//        }
//
//        uint8_t new = in[i];
//
//        new <<= (8 - shift);
//        new |= ((prev & (0xFF << shift)) >> shift);
//        new &= ~0x80;
//        switch (state)
//        {
//        case 0:
//        	if (new == 27)
//        	{
//        		state = 1;
//        	}
//        	else out[j] = new;
//        	break;
//        case 1:
//        	state = 0;
//        	switch (new)
//        	{
//            case    10:
//                out[j]=12;
//                break;
//            case    20:
//                out[j]='^';
//                break;
//            case    40:
//                out[j]='{';
//                break;
//            case    41:
//                out[j]='}';
//                break;
//            case    47:
//                out[j]='\\';
//                break;
//            case    60:
//                out[j]='[';
//                break;
//            case    61:
//                out[j]='~';
//                break;
//            case    62:
//                out[j]=']';
//                break;
//           case    64:
//                out[j]='|';
//                break;
//            default:
//                out[j]='?';
//                break;
//
//        	}
//        	break;
//        }
//
//        prev = in[i];
//    }
//
//    return 0;
//}

//TODO: special character
//static int encode7(uint8_t *in, size_t in_sz, uint8_t *out, size_t out_sz)
//{
//    size_t out_len_max = ((in_sz * 7) / 8 + 1);
//
//    if ( out_len_max > out_sz ) {
//        return -1;
//    }
//
//    int j = 0;
//    for ( int i = 0, shift = 7; (i < in_sz) && (j < out_sz); ++i, ++j, --shift ) {
//
//        uint8_t new = in[i];
//        new &= 0x7F;
//        new >>= (7 - shift);
//
//        uint8_t next = 0x00;
//        if ( i < (in_sz - 1) ) {
//            next = in[i + 1];
//            new |= ((next & (0xFF >> shift)) << shift);
//        }
//
//        out[j] = new;
//
//        if ( shift == 1 ) {
//            ++i;
//            shift = 8;
//        }
//
//    }
//
//    return j;
//}

static size_t decode_stroctet(const char *octet, size_t octet_sz, uint8_t *data, size_t data_sz)
{
    for ( int i = 0, j = 0; (i < octet_sz) && (j < data_sz); i += 2, ++j ) {
        uint value;

        sscanf(octet + i, "%02X", &value);
        data[j] = (uint8_t) value;
    }

    return (octet_sz / 2 < data_sz ? octet_sz / 2 : data_sz);
}

static size_t encode_stroctet(const uint8_t *data, size_t data_sz, char *octet, size_t octet_sz)
{
    if ( octet_sz < (data_sz * 2) ) {
        return 0;
    }

    for ( int i = 0, j = 0; (i < octet_sz) && (j < data_sz); i += 2, ++j ) {
        uint8_t d = data[j];

        printf("%02X", d);

        octet[i] = hex2c((d & 0xF0) >> 4);
        octet[i + 1] = hex2c(d & 0x0F);
    }

    return (data_sz * 2);
}

static int decode_telnum(const char *num, size_t num_sz, sms_t *sms)
{
    if ( (num_sz % 2) != 0 ) {
        return -1;
    }

    if ( num_sz > SMS_SENDER_SIZE ) {
        return -2;
    }

    for ( int i = 0, j = 0; i < num_sz; i += 2 ) {
        char n1 = num[i];
        char n2 = num[i + 1];

        sms->telnum[j] = n2;
        ++j;

        if ( n1 != 'F' ) {
            sms->telnum[j] = n1;
            ++j;
        }
    }

    return 0;
}

static int encode_telnum(const char *num, size_t num_sz, uint8_t *buf, size_t buf_sz)
{
    if ( buf_sz < (num_sz / 2) ) {
        return -1;
    }

    int j = 0;
    for ( int i = 0; i < num_sz; i += 2, j++ ) {
        uint8_t n1 = (num[i] - '0');
        uint8_t n2 = (num[i + 1] - '0');

        buf[j] = ((n2 << 4) | n1);
    }

    if ( (num_sz % 2) != 0 ) {
        uint8_t n1 = (num[num_sz - 1] - '0');
        buf[j] = (0xF0 | n1);
    }

    return j;
}

int pdu2text(const char *pdu, char *sms_text, int *text_length, int *expected_length,
             int with_udh, char *udh, char *udh_type, int *errorpos)
{
    int charcounter;
    int septets;
    int octets;
    int udhsize;
    int octetcounter;
    int skip_characters = 0;
    char c;
    int i;
    char text[SMS_MESSAGE_SIZE];

    if (udh)
        *udh = 0;
    if (udh_type)
        *udh_type = 0;

    sscanf(pdu, "%02X", &septets);

    if (with_udh)
    {
        // copy the data header to udh and convert to hex dump
        // There was at least one octet and next will give an error if there is no more data:
        sscanf(pdu + 2, "%02X", &udhsize);

        for (octetcounter = 0; octetcounter < udhsize + 1; octetcounter++)
        {
            udh[octetcounter*3]=pdu[(octetcounter<<1)+2];

            udh[octetcounter*3+1]=pdu[(octetcounter<<1)+3];

            udh[octetcounter *3 +2] = ' ';
            udh[octetcounter *3 +3] = 0;
        }

        // Calculate how many text charcters include the UDH.
        // After the UDH there may follow filling bits to reach a 7bit boundary.
        skip_characters=(((udhsize+1)*8)+6)/7;
    }

    if (expected_length)
        *expected_length = septets - skip_characters;

    // Convert from 8-Bit to 7-Bit encapsulated in 8 bit
    // skipping storing of some characters used by UDH.
    // 3.1beta7: Simplified handling to allow partial decodings to be shown.
    octets = (septets *7 +7) /8;

    /* decode octet string to binary data */
    uint8_t decode[SMS_MESSAGE_SIZE];
    size_t decode_sz = decode_stroctet(pdu + 2, octets * 2, decode, SMS_MESSAGE_SIZE);

	uint8_t prev = 0;
	int shift;
	for (i = 0, charcounter = 0, shift = 8; (i < septets) && (charcounter < septets);
			++i, ++charcounter, --shift)
	{
		if (shift == 1)
		{
			text[charcounter] = (prev >> 1);
			++charcounter;
			shift = 8;
		}

		uint8_t	c = decode[i];

		c <<= (8 - shift);
		c |= ((prev & (0xFF << shift)) >> shift);
		c &= ~0x80;
		text[charcounter] = c;

		prev = decode[i];
	}

	int j = 0;
	for (int i = skip_characters; i < (septets - skip_characters); i++)
	{
		if (text[i] != 27)
		{
			sms_text[j] = text[i];
		}
		else
		{
			i++;
			switch (text[i])
			{
			case 10:
				sms_text[j] = 12;
				break;
			case 20:
				sms_text[j] = '^';
				break;
			case 40:
				sms_text[j] = '{';
				break;
			case 41:
				sms_text[j] = '}';
				break;
			case 47:
				sms_text[j] = '\\';
				break;
			case 60:
				sms_text[j] = '[';
				break;
			case 61:
				sms_text[j] = '~';
				break;
			case 62:
				sms_text[j] = ']';
				break;
			case 64:
				sms_text[j] = '|';
				break;
			default:
				sms_text[j] = '?';
				break;
			}
		}
		j++;
	}
	sms_text[j] = NULL;

	*text_length = j;

    return charcounter -skip_characters;
}

int sms_decode_pdu(const char *data, size_t sz, sms_t *sms)
{
    if ( !data ) {
        return -1;
    }
    if ( !sms ) {
        return -1;
    }

    uint8_t isUDH = 0;

    /* reset sms struct */
    bzero(sms, sizeof (sms_t));

    const char *pdata = data;

    uint32_t smc_length;
    sscanf(pdata, "%02X", &smc_length);
    pdata += 2;
    /* skip SMC information */
    pdata += (smc_length * 2);

    uint32_t sms_deliver;
    sscanf(pdata, "%02X", &sms_deliver);
    if ((sms_deliver & 0x40) == 0x40)
    {
    	isUDH = 1;
    	printf("\nWith UDH\n");
    }
    pdata += 2;

    /* read sender number length */
    uint32_t num_length;
    sscanf(pdata, "%02X", &num_length);
    pdata += 2;
    sms->telnum_length = num_length;
    num_length = ((num_length % 2) == 0 ? num_length : num_length + 1);

    /* read type of address of the sender number */
    uint32_t sender_addr_type;
    sscanf(pdata, "%02X", &sender_addr_type);
    pdata += 2;
    sms->telnum_type = sender_addr_type;

    /* read phone number */
    decode_telnum(pdata, num_length, sms);
    pdata += num_length;

    /* skip protocol identifier */
    pdata += 2;

    /* read data coding scheme */
    unsigned int data_coding;
    sscanf(pdata, "%02X", &data_coding);
    pdata += 2;

    /* skip timestamp */
    pdata += 14;

    /* read user data length */
    uint32_t user_data_length;
    sscanf(pdata, "%02X", &user_data_length);
//    pdata += 2;
//    sms->message_length = user_data_length;
    pdu2text(pdata, sms->message, (int *)&sms->message_length, 0, (int)isUDH, sms->udh, 0, 0);

    return isUDH;

//    int decode_success = 0;
//    if ( data_coding == 0x00 ) {
//
//        /* decode octet string to binary data */
//        uint8_t decode[SMS_MESSAGE_SIZE];
//        size_t decode_sz = decode_stroctet(pdata, user_data_length * 2, decode, SMS_MESSAGE_SIZE);
//
//        if ( decode_sz >= 0 ) {
//
//            /* decode 8 bit to 7 bit */
//            int dec7_res = decode7(decode, user_data_length, sms->message, SMS_MESSAGE_SIZE);
//
//            if ( dec7_res == 0 ) {
//                /* set data length */
//                sms->message_length = user_data_length;
//                decode_success = 1;
//            }
//        }
//    }
//    else if ( data_coding == 0x04 ) {
//        /* decode octet string to binary data */
//        size_t decode_sz = decode_stroctet(pdata, user_data_length * 2, sms->message, SMS_MESSAGE_SIZE);
//        if ( decode_sz >= 0 ) {
//            /* set data length */
//            sms->message_length = decode_sz;
//            decode_success = 1;
//        }
//    }
//    else if ( data_coding == 0x08 ) {
//        /* decode octet string to binary data */
//        size_t decode_sz = decode_stroctet(pdata, user_data_length * 4, sms->message, SMS_MESSAGE_SIZE);
//        if ( decode_sz >= 0 ) {
//            /* set data length */
//            sms->message_length = decode_sz;
//            decode_success = 1;
//        }
//    }
//
//    return (decode_success ? 0 : -1);
}

//int sms_encode_pdu(sms_t *sms, char *data, size_t sz)
//{
//    if ( !data ) {
//        return -1;
//    }
//    if ( !sms ) {
//        return -1;
//    }
//
//    bzero(data, sz);
//
//    size_t buf_sz = 9 + ((sms->telnum_length % 2) == 0 ? sms->telnum_length : sms->telnum_length + 1) / 2 + ((sms->message_length * 7) / 8 + 1);
//
//    if ( sz < (buf_sz * 2) ) {
//        return -1;
//    }
//
//    uint8_t *buf = (uint8_t *) malloc(buf_sz * sizeof (uint8_t));
//    if ( !buf ) {
//        return -1;
//    }
//
//    uint8_t *pbuf = buf;
//    *(pbuf++) = 0x00; /* Length of SMSC information */
//    *(pbuf++) = 0x11; /* First octet of the SMS-SUBMIT message. */
//    *(pbuf++) = 0x00; /* TP-Message-Reference. The "00" value here lets the phone set the message reference number itself. */
//    *(pbuf++) = (uint8_t) sms->telnum_length;
//    *(pbuf++) = sms->telnum_type;
//    int num_sz = encode_telnum(sms->telnum, sms->telnum_length, pbuf, buf_sz - (pbuf - buf));
//    if ( num_sz >= 0 ) {
//        pbuf += num_sz;
//    }
//    *(pbuf++) = 0x00; /* TP-PID. Protocol identifier */
//    *(pbuf++) = 0x00; /* TP-DCS. Data coding scheme.This message is coded according to the 7bit default alphabet. */
//    *(pbuf++) = 0xAA; /* TP-Validity-Period. "AA" means 4 days. */
//    int enc7_sz = encode7(sms->message, sms->message_length, pbuf + 1, buf_sz - (pbuf - buf) - 1);
//    if ( enc7_sz >= 0 ) {
//        *(pbuf++) = (uint8_t) sms->message_length;
//        pbuf += enc7_sz;
//    }
//    else {
//        *(pbuf++) = 0;
//    }
//
//    size_t enc_sz = encode_stroctet(buf, (pbuf - buf), data, sz);
//
//    free(buf);
//
//    return enc_sz;
//}

ssize_t sms_write(const char *mesg, sms_t *sms)
{
    if ( !mesg ) {
        return -1;
    }
    if ( !sms ) {
        return -1;
    }

    size_t mesg_len = strlen(mesg);

    if ( mesg_len > SMS_MESSAGE_SIZE ) {
        mesg_len = SMS_MESSAGE_SIZE;
    }

    memcpy(sms->message, mesg, mesg_len);
    sms->message_length = mesg_len;

    return mesg_len;
}
