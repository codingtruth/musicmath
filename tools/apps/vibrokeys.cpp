#include "vibro.h"

int main()
{
    VibroStream stream;
    char c;
    double p0;
    p0 = 5.0/8.0 * M_PI / (216.0 * 16.0);
    double l;
    l = 1000.0 * M_PI*M_PI;
    double p;
    p = p0;
    stream.play();

    while (stream.getStatus() == VibroStream::Playing)
    {
        std::cout << "p = " << p / M_PI << " l = " << l / (M_PI * M_PI) << "\n";
        c = getch();
        switch (c){
        case 'z':
            return 0;
		case 'x':
			stream.save_start("tmp.wav");
			break;
        case '`':
        	p = p0;
        	break;
#if 1
        case '1':
        	p = p0 * 1.0/1.0;
        	break;
        case '2':
        	p = p0 * 9.0/10.0;
        	break;
        case '3':
        	p = p0 * 4.0/5.0;
        	break;
        case '4':
        	p = p0 * 3.0/4.0;
        	break;
		case '5':
			p = p0 * 2.0/3.0;
			break;
		case '6':
			p = p0 * 3.0/5.0;
			break;
		case '7':
			p = p0 * 27.0/50.0;
			break;
		case '8':
			p = p0 * 1.0/2.0;
			break;
		case '9':
			p = p0 * 1.0/2.0 * 9.0/10.0;
			break;
		case '0':
			p = p0 * 1.0/2.0 * 4.0/5.0;
			break;
        case '-':
        	p = p0 * 1.0/2.0 * 3.0/4.0;
        	break;
        case '=':
        	p = p0 * 1.0/2.0 * 2.0/3.0;
        	break;

        case 'q':
        	p = p0 * 1.0/1.0;
        	break;
        case 'w':
        	p = p0 * 8.0/9.0;
        	break;
        case 'e':
        	p = p0 * 5.0/6.0;
        	break;
        case 'r':
        	p = p0 * 3.0/4.0;
        	break;
		case 't':
			p = p0 * 2.0/3.0;
			break;
		case 'y':
			p = p0 * 5.0/8.0;
			break;
		case 'u':
			p = p0 * 5.0/9.0;
			break;
		case 'i':
			p = p0 * 1.0/2.0;
			break;
		case 'o':
			p = p0 * 1.0/2.0 * 8.0/9.0;
			break;
		case 'p':
			p = p0 * 1.0/2.0 * 5.0/6.0;
			break;
        case '[':
            p = p0 * 1.0/2.0 * 3.0/4.0;
            break;
        case ']':
            p = p0 * 1.0/2.0 * 2.0/3.0;
            break;
#else

        case 'q':
        	p = p0 * 2.0/3.0 * 2; // 2V
        	break;
        case 'r':
        	p = p0 * 1.0/1.0;
        	break;
        case '5':
        	p = p0 * 15.0/16.0;   // IId
        	break;
        case 't':
        	p = p0 * 9.0/10.0;    // IIb
        	break;
        case '6':
        	p = p0 * 8.0/9.0;     // II
        	break;
        case 'y':
        	p = p0 * 7.0/8.0;     // s
        	break;
        case '7':
        	p = p0 * 6.0/7.0;     // s
        	break;
        case 'u':
        	p = p0 * 5.0/6.0;     // III
        	break;
        case '8':
        	p = p0 * 4.0/5.0;     // IIIm
        	break;
        case 'i':
        	p = p0 * 3.0/4.0;     // IV
        	break;
        case '9':
        	p = p0 * 20.0/27.0;   // IVp
        	break;
        case 'o':
        	p = p0 * 20.0/27.0;   // IVq
        	break;
        case '0':
        	p = p0 * 45.0/64.0;   // Vd
        	break;
        case 'p':
        	p = p0 * 27.0/40.0;   // Vb
        	break;
        case '-':
        	p = p0 * 2.0/3.0;     // V
        	break;
#endif
        }
        stream.pick(p);//, l);
    }

    return 0;
}
