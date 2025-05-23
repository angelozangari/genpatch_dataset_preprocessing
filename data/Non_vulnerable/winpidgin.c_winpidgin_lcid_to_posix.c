}
static wchar_t* winpidgin_lcid_to_posix(LCID lcid) {
	wchar_t *posix = NULL;
	int lang_id = PRIMARYLANGID(lcid);
	int sub_id = SUBLANGID(lcid);
	switch (lang_id) {
		case LANG_AFRIKAANS: posix = L"af"; break;
		case LANG_ARABIC: posix = L"ar"; break;
		case LANG_AZERI: posix = L"az"; break;
		case LANG_BENGALI: posix = L"bn"; break;
		case LANG_BULGARIAN: posix = L"bg"; break;
		case LANG_CATALAN: posix = L"ca"; break;
		case LANG_CZECH: posix = L"cs"; break;
		case LANG_DANISH: posix = L"da"; break;
		case LANG_ESTONIAN: posix = L"et"; break;
		case LANG_PERSIAN: posix = L"fa"; break;
		case LANG_GERMAN: posix = L"de"; break;
		case LANG_GREEK: posix = L"el"; break;
		case LANG_ENGLISH:
			switch (sub_id) {
				case SUBLANG_ENGLISH_UK:
					posix = L"en_GB"; break;
				case SUBLANG_ENGLISH_AUS:
					posix = L"en_AU"; break;
				case SUBLANG_ENGLISH_CAN:
					posix = L"en_CA"; break;
				default:
					posix = L"en"; break;
			}
			break;
		case LANG_SPANISH: posix = L"es"; break;
		case LANG_BASQUE: posix = L"eu"; break;
		case LANG_FINNISH: posix = L"fi"; break;
		case LANG_FRENCH: posix = L"fr"; break;
		case LANG_GALICIAN: posix = L"gl"; break;
		case LANG_GUJARATI: posix = L"gu"; break;
		case LANG_HEBREW: posix = L"he"; break;
		case LANG_HINDI: posix = L"hi"; break;
		case LANG_HUNGARIAN: posix = L"hu"; break;
		case LANG_ICELANDIC: break;
		case LANG_INDONESIAN: posix = L"id"; break;
		case LANG_ITALIAN: posix = L"it"; break;
		case LANG_JAPANESE: posix = L"ja"; break;
		case LANG_GEORGIAN: posix = L"ka"; break;
		case LANG_KANNADA: posix = L"kn"; break;
		case LANG_KOREAN: posix = L"ko"; break;
		case LANG_LITHUANIAN: posix = L"lt"; break;
		case LANG_MACEDONIAN: posix = L"mk"; break;
		case LANG_DUTCH: posix = L"nl"; break;
		case LANG_NEPALI: posix = L"ne"; break;
		case LANG_NORWEGIAN:
			switch (sub_id) {
				case SUBLANG_NORWEGIAN_BOKMAL:
					posix = L"nb"; break;
				case SUBLANG_NORWEGIAN_NYNORSK:
					posix = L"nn"; break;
			}
			break;
		case LANG_PUNJABI: posix = L"pa"; break;
		case LANG_POLISH: posix = L"pl"; break;
		case LANG_PASHTO: posix = L"ps"; break;
		case LANG_PORTUGUESE:
			switch (sub_id) {
				case SUBLANG_PORTUGUESE_BRAZILIAN:
					posix = L"pt_BR"; break;
				default:
				posix = L"pt"; break;
			}
			break;
		case LANG_ROMANIAN: posix = L"ro"; break;
		case LANG_RUSSIAN: posix = L"ru"; break;
		case LANG_SLOVAK: posix = L"sk"; break;
		case LANG_SLOVENIAN: posix = L"sl"; break;
		case LANG_ALBANIAN: posix = L"sq"; break;
		/* LANG_CROATIAN == LANG_SERBIAN == LANG_BOSNIAN */
		case LANG_SERBIAN:
			switch (sub_id) {
				case SUBLANG_SERBIAN_LATIN:
					posix = L"sr@Latn"; break;
				case SUBLANG_SERBIAN_CYRILLIC:
					posix = L"sr"; break;
				case SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_CYRILLIC:
				case SUBLANG_BOSNIAN_BOSNIA_HERZEGOVINA_LATIN:
					posix = L"bs"; break;
				case SUBLANG_CROATIAN_BOSNIA_HERZEGOVINA_LATIN:
					posix = L"hr"; break;
			}
			break;
		case LANG_SWEDISH: posix = L"sv"; break;
		case LANG_TAMIL: posix = L"ta"; break;
		case LANG_TELUGU: posix = L"te"; break;
		case LANG_THAI: posix = L"th"; break;
		case LANG_TURKISH: posix = L"tr"; break;
		case LANG_UKRAINIAN: posix = L"uk"; break;
		case LANG_VIETNAMESE: posix = L"vi"; break;
		case LANG_XHOSA: posix = L"xh"; break;
		case LANG_CHINESE:
			switch (sub_id) {
				case SUBLANG_CHINESE_SIMPLIFIED:
					posix = L"zh_CN"; break;
				case SUBLANG_CHINESE_TRADITIONAL:
					posix = L"zh_TW"; break;
				default:
					posix = L"zh"; break;
			}
			break;
		case LANG_URDU: break;
		case LANG_BELARUSIAN: break;
		case LANG_LATVIAN: break;
		case LANG_ARMENIAN: break;
		case LANG_FAEROESE: break;
		case LANG_MALAY: break;
		case LANG_KAZAK: break;
		case LANG_KYRGYZ: break;
		case LANG_SWAHILI: break;
		case LANG_UZBEK: break;
		case LANG_TATAR: break;
		case LANG_ORIYA: break;
		case LANG_MALAYALAM: break;
		case LANG_ASSAMESE: break;
		case LANG_MARATHI: break;
		case LANG_SANSKRIT: break;
		case LANG_MONGOLIAN: break;
		case LANG_KONKANI: break;
		case LANG_MANIPURI: break;
		case LANG_SINDHI: break;
		case LANG_SYRIAC: break;
		case LANG_KASHMIRI: break;
		case LANG_DIVEHI: break;
	}
	/* Deal with exceptions */
	if (posix == NULL) {
		switch (lcid) {
			case 0x0455: posix = L"my_MM"; break; /* Myanmar (Burmese) */
			case 9999: posix = L"ku"; break; /* Kurdish (from NSIS) */
		}
	}
	return posix;
}
