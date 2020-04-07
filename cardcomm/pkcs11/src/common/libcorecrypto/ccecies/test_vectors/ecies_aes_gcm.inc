// P192, SHA1, no diversification
{
    .di=di_SHA1,
    .curve=&ccec_cp_192,
    .mac_length = 16,
    .key_length = 16,
    // Decryption private key
    .dec_priv_key = "9ebc0975033b0330bc461613ddd59a9e42ea06efb1997dc6",
    //Qsx = 5f906901924c87fa6b24ec4b8f6fd22bb8b78f30f6a32738
    //Qsy = f00cb3613ad1ec9bb667db4167f8464749c519eb3e12acde
    // Ephemeral private key
    .eph_priv_key = "5c7d5155376ed3cef3f851314cca479187771c89edd7ad13",
    //Qex = fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558a
    //Qey = 48d1ea67729bc3ee1a55d3e889865777c5ca99e2045ab4c9
    //Z = f1b9ec8da4c8a8d7ed27bfe2a798646cb64fcc37f2504cf7
    //KDF(Z,00000001,"") = 99fbfc5106a564cf425d64a46d068327
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = 10edae238eadea475b4c4b0b3c
    // Mac Tag = 149ad359e4af0118ee89e6165342c164
    .cipher = "04fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558a48d1ea67729bc3ee1a55d3e889865777c5ca99e2045ab4c910edae238eadea475b4c4b0b3c149ad359e4af0118ee89e6165342c164",
    .compact_cipher = "fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558a10edae238eadea475b4c4b0b3c149ad359e4af0118ee89e6165342c164",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P192, SHA1, diversification SharedInfo 1 & 2
{
    .di=di_SHA1,
    .curve=&ccec_cp_192,
    .mac_length = 16,
    .key_length = 16,
    // Decryption private key
    .dec_priv_key = "9ebc0975033b0330bc461613ddd59a9e42ea06efb1997dc6",
    //Qsx = 5f906901924c87fa6b24ec4b8f6fd22bb8b78f30f6a32738
    //Qsy = f00cb3613ad1ec9bb667db4167f8464749c519eb3e12acde
    // Ephemeral private key
    .eph_priv_key = "5c7d5155376ed3cef3f851314cca479187771c89edd7ad13",
    //Qex = fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558a
    //Qey = 48d1ea67729bc3ee1a55d3e889865777c5ca99e2045ab4c9
    //Z = f1b9ec8da4c8a8d7ed27bfe2a798646cb64fcc37f2504cf7
    //KDF(Z,00000001,"Test1") = b78241943751f594dde08fda421e67e9
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = ca9be2de14349990966e205652
    // Mac Tag = 5d8e885a160b77af44e35768735b622f
    .cipher = "04fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558a48d1ea67729bc3ee1a55d3e889865777c5ca99e2045ab4c9ca9be2de14349990966e2056525d8e885a160b77af44e35768735b622f",
    .compact_cipher = "fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558aca9be2de14349990966e2056525d8e885a160b77af44e35768735b622f",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="Test1",
    .sharedInfo2="Test2",
},
// P192, SHA1, diversification SharedInfo 1 = Ephemeral public point
{
    .di=di_SHA1,
    .curve=&ccec_cp_192,
    .mac_length = 16,
    .key_length = 16,
    // Decryption private key
    .dec_priv_key = "9ebc0975033b0330bc461613ddd59a9e42ea06efb1997dc6",
    //Qsx = 5f906901924c87fa6b24ec4b8f6fd22bb8b78f30f6a32738
    //Qsy = f00cb3613ad1ec9bb667db4167f8464749c519eb3e12acde
    // Ephemeral private key
    .eph_priv_key = "5c7d5155376ed3cef3f851314cca479187771c89edd7ad13",
    //Qex = fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558a
    //Qey = 48d1ea67729bc3ee1a55d3e889865777c5ca99e2045ab4c9
    //Z = f1b9ec8da4c8a8d7ed27bfe2a798646cb64fcc37f2504cf7
    //KDF(Z,00000001,04fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558a48d1ea67729bc3ee1a55d3e889865777c5ca99e2045ab4c9
    //    ) = 86db0e3606b7e886f19418f22ac309c3
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = 23a9fd8cee479aab996b2f74f0
    // Mac Tag = 3c9642dbec29b7296bd8ad664aa2d37a
    .cipher = "04fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558a48d1ea67729bc3ee1a55d3e889865777c5ca99e2045ab4c923a9fd8cee479aab996b2f74f03c9642dbec29b7296bd8ad664aa2d37a",
    .compact_cipher= "fd2af653b5de1437ba0c4b3b301e00909aeee462a01a558af4dff8e3a4fd307e284d2e786ea8176079b0617a69c32fed7c386c791f",
    .options=ECIES_EXPORT_PUB_STANDARD+ECIES_EPH_PUBKEY_IN_SHAREDINFO1,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P224, SHA224, no diversification
{
    .di=di_SHA224,
    .curve=&ccec_cp_224,
    .mac_length = 16,
    .key_length = 16,
    // Decryption private key
    .dec_priv_key = "11f0abdf4170f59ef4fca7871490f34902084ca84e299bedf0958401",
    //QeCAVSx = 60aa94e6b899069c6711221a332d3156168537a085011efe054d4cdf
    //QeCAVSy = 9ac019527cf4a7a4cd084856e80462f9fab7b8bd12b37d1b31bf03a5
    .eph_priv_key = "0e6d170df9aeeeaf4c14f734745ac83bf44b4b2e2419c44cb9dba4d2",
    //QsIUTx = 91ac459a13e4ee58306d02eb7cfb065d253ac0a07698dae19272c989
    //QsIUTy = d68c441be99223d911fe8da0277c10378943bc03725af4b77d844516
    //Z = 398b22a9a4c6da8f6677ef14994a057de60da78126bbd77e70e8e4e4
    //KDF(Z,00000001,"") = 20f5ebd7d2c72dec6dbca62834f566c9
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = 96ccd1189b1b7d14ec02cae2b5
    // Mac Tag = ecef8f5572b127252cc9a4cf78455a33
    .cipher = "0491ac459a13e4ee58306d02eb7cfb065d253ac0a07698dae19272c989d68c441be99223d911fe8da0277c10378943bc03725af4b77d84451696ccd1189b1b7d14ec02cae2b5ecef8f5572b127252cc9a4cf78455a33",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P224, SHA224, diversification SharedInfo 1 & 2
{
    .di=di_SHA224,
    .curve=&ccec_cp_224,
    .mac_length = 16,
    .key_length = 16,
    .dec_priv_key = "11f0abdf4170f59ef4fca7871490f34902084ca84e299bedf0958401",
    //QeCAVSx = 60aa94e6b899069c6711221a332d3156168537a085011efe054d4cdf
    //QeCAVSy = 9ac019527cf4a7a4cd084856e80462f9fab7b8bd12b37d1b31bf03a5
    .eph_priv_key = "0e6d170df9aeeeaf4c14f734745ac83bf44b4b2e2419c44cb9dba4d2",
    //QsIUTx = 91ac459a13e4ee58306d02eb7cfb065d253ac0a07698dae19272c989
    //QsIUTy = d68c441be99223d911fe8da0277c10378943bc03725af4b77d844516
    //Z = 398b22a9a4c6da8f6677ef14994a057de60da78126bbd77e70e8e4e4
    //KDF(Z,00000001,"Test1") = 0ac4e89cfc3fe709b3deed76b17105b8
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = b28bdade5d3707be00b98f0030
    // Mac Tag = 23f559c1c6fbf40138b30db7a2c8a61f
    .cipher = "0491ac459a13e4ee58306d02eb7cfb065d253ac0a07698dae19272c989d68c441be99223d911fe8da0277c10378943bc03725af4b77d844516b28bdade5d3707be00b98f003023f559c1c6fbf40138b30db7a2c8a61f",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="Test1",
    .sharedInfo2="Test2",
},
// P224, SHA224, diversification SharedInfo 1 = Ephemeral public point
{
    .di=di_SHA224,
    .curve=&ccec_cp_224,
    .mac_length = 16,
    .key_length = 16,
    .dec_priv_key = "11f0abdf4170f59ef4fca7871490f34902084ca84e299bedf0958401",
    //QeCAVSx = 60aa94e6b899069c6711221a332d3156168537a085011efe054d4cdf
    //QeCAVSy = 9ac019527cf4a7a4cd084856e80462f9fab7b8bd12b37d1b31bf03a5
    .eph_priv_key = "0e6d170df9aeeeaf4c14f734745ac83bf44b4b2e2419c44cb9dba4d2",
    //QsIUTx = 91ac459a13e4ee58306d02eb7cfb065d253ac0a07698dae19272c989
    //QsIUTy = d68c441be99223d911fe8da0277c10378943bc03725af4b77d844516
    //Z = 398b22a9a4c6da8f6677ef14994a057de60da78126bbd77e70e8e4e4
    //KDF(Z,0491ac459a13e4ee58306d02eb7cfb065d253ac0a07698dae19272c989d68c441be99223d911fe8da0277c10378943bc03725af4b77d844516
    //    ) = c27836e8c57002dd0235a169261cba85
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = 9267f3088082a906b46e54fc59
    // Mac Tag = 2787ed6b2fd85f1c0b51126440f118ac
    .cipher = "0491ac459a13e4ee58306d02eb7cfb065d253ac0a07698dae19272c989d68c441be99223d911fe8da0277c10378943bc03725af4b77d8445169267f3088082a906b46e54fc592787ed6b2fd85f1c0b51126440f118ac",
    .options=ECIES_EXPORT_PUB_STANDARD+ECIES_EPH_PUBKEY_IN_SHAREDINFO1,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P256, SHA256, no diversification
{
    .di=di_SHA256,
    .curve=&ccec_cp_256,
    .mac_length = 16,
    .key_length = 16,
	.dec_priv_key = "5abf13dbd59e0c5872bd9e72b4e17c5c9e6fe956b51aca389ad43ff5594c82b3",
	//QeCAVSx = 230951e9f774dae7c7938fcfe407a54981bda3b6b79bb37d82823f9b4ae065d5
	//QeCAVSy = 4b30c7314c1b96b0554e26e95d641408df645349a14090cd672ae9cc5829a3c4
	.eph_priv_key = "1ffe7e4cbbb59f069806ebbcd18f66bc42ac3cfc9b8d0d61c7a50e94f71cf6b5",
	//QsIUTx = 895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451d
	//QsIUTy = aebf853151e56f3ed04b1e68bacb39c4c8e140c10cfca6e419478b07946abdab
	//Z = fa17d1d5717a75202a3e02ea665322f3566407d631c93a06ba278ba68a2f2c45,
    //KDF(Z,00000001,"") = 6f2e784ff28f857a87d0601e9dc6a3ba
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = f7506516305ad50b032610c46d
    // Mac Tag = 1d5c7e7f438b8eb19f1b488821901bde
    .cipher = "04895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451daebf853151e56f3ed04b1e68bacb39c4c8e140c10cfca6e419478b07946abdabf7506516305ad50b032610c46d1d5c7e7f438b8eb19f1b488821901bde",
    .compact_cipher = "895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451df7506516305ad50b032610c46d1d5c7e7f438b8eb19f1b488821901bde",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P256, SHA256, diversification SharedInfo 1 & 2
{
    .di=di_SHA256,
    .curve=&ccec_cp_256,
    .mac_length = 16,
    .key_length = 16,
	.dec_priv_key = "5abf13dbd59e0c5872bd9e72b4e17c5c9e6fe956b51aca389ad43ff5594c82b3",
	//QeCAVSx = 230951e9f774dae7c7938fcfe407a54981bda3b6b79bb37d82823f9b4ae065d5
	//QeCAVSy = 4b30c7314c1b96b0554e26e95d641408df645349a14090cd672ae9cc5829a3c4
	.eph_priv_key = "1ffe7e4cbbb59f069806ebbcd18f66bc42ac3cfc9b8d0d61c7a50e94f71cf6b5",
	//QsIUTx = 895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451d
	//QsIUTy = aebf853151e56f3ed04b1e68bacb39c4c8e140c10cfca6e419478b07946abdab
	//Z = fa17d1d5717a75202a3e02ea665322f3566407d631c93a06ba278ba68a2f2c45,
    //KDF(Z,00000001,"Test1") = 815a8e9607dc6d0dd3c7d7cafe931a9c
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = 3cf778f2bc04da2d3657391669
    // Mac Tag = eaf761b9bd6895fc7660f70113870d15
    .cipher = "04895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451daebf853151e56f3ed04b1e68bacb39c4c8e140c10cfca6e419478b07946abdab3cf778f2bc04da2d3657391669eaf761b9bd6895fc7660f70113870d15",
    .compact_cipher = "895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451d3cf778f2bc04da2d3657391669eaf761b9bd6895fc7660f70113870d15",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="Test1",
    .sharedInfo2="Test2",
},
// P256, SHA256, diversification SharedInfo 1 = Ephemeral public point
{
    .di=di_SHA256,
    .curve=&ccec_cp_256,
    .mac_length = 16,
    .key_length = 16,
	.dec_priv_key = "5abf13dbd59e0c5872bd9e72b4e17c5c9e6fe956b51aca389ad43ff5594c82b3",
	//QeCAVSx = 230951e9f774dae7c7938fcfe407a54981bda3b6b79bb37d82823f9b4ae065d5
	//QeCAVSy = 4b30c7314c1b96b0554e26e95d641408df645349a14090cd672ae9cc5829a3c4
	.eph_priv_key = "1ffe7e4cbbb59f069806ebbcd18f66bc42ac3cfc9b8d0d61c7a50e94f71cf6b5",
	//QsIUTx = 895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451d
	//QsIUTy = aebf853151e56f3ed04b1e68bacb39c4c8e140c10cfca6e419478b07946abdab
	//Z = fa17d1d5717a75202a3e02ea665322f3566407d631c93a06ba278ba68a2f2c45,
    //KDF(Z,04895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451daebf853151e56f3ed04b1e68bacb39c4c8e140c10cfca6e419478b07946abdab
    //    ) = 864c47a851501ade4b749f1c4dbf02d4
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = e658d7f2ebafe7e3cef9bcbbb5
    // Mac Tag = 19dee0db770eb72b4143a9df85e98a63
    .cipher = "04895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451daebf853151e56f3ed04b1e68bacb39c4c8e140c10cfca6e419478b07946abdabe658d7f2ebafe7e3cef9bcbbb519dee0db770eb72b4143a9df85e98a63",
    .compact_cipher = "895ce51cabc5cc0f857504bc684106290335538298b9de9c4759a8b3945f451de3730a7db1f52354858c1457f147e901c84e3ee0a78367411743a285cb",
    .options=ECIES_EXPORT_PUB_STANDARD+ECIES_EPH_PUBKEY_IN_SHAREDINFO1,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P384, SHA384, no diversification
{
    .di=di_SHA384,
    .curve=&ccec_cp_384,
    .mac_length = 16,
    .key_length = 16,
    .dec_priv_key = "a826c4636474b208605393fdc4f664332ef8a3594474909f5e093738d893cc56431d223b1cd403d725e98058bbff9d02",
    //QeCAVSx = 81a5f71f204c3f4a96fe5aea9ceecfbedea8accf18925465bcb87b5fdb566079d4c909cb900efd2a7e6891bf91cc3afb
    //QeCAVSy = 82ae0a4b3b94445a5658326805a0501787f34249eb0ec63f1b06d0e05bc72e7888654f94af49cb425a8c1927bc4ad741
    .eph_priv_key = "00eb5222e7e73c887e45df9cf54b10e53a6a1ed8ab98a8363cd35fb0cdc5c05dee0642c71addaee88be7e81385f14060",
    //QsIUTx = 55c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d38
    //QsIUTy = 84b6258ec0079c4ac4e2efcd848ffc74fb2972a0228aed059549e04a9ab021dddf6d6fee19d7e5e4406bc49ad1023c20
    //Z = deb56295d036ce1abe61070aba1435102665b911b64188a8ed9778c2ecf1606d51416d9370f5665f8525694753416e26
    //KDF(Z,00000001,"") = 65073737cf26a082d57dad28f1f55aa0
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = 6a1b69072246b2c6289d62d628
    // Mac Tag = a0a3633baf2c1a8014fc9f5d2bb6d9a9
    .cipher = "0455c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d3884b6258ec0079c4ac4e2efcd848ffc74fb2972a0228aed059549e04a9ab021dddf6d6fee19d7e5e4406bc49ad1023c206a1b69072246b2c6289d62d628a0a3633baf2c1a8014fc9f5d2bb6d9a9",
    .compact_cipher = "55c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d386a1b69072246b2c6289d62d628a0a3633baf2c1a8014fc9f5d2bb6d9a9",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P384, SHA384, diversification SharedInfo 1 & 2
{
    .di=di_SHA384,
    .curve=&ccec_cp_384,
    .mac_length = 16,
    .key_length = 16,
    .dec_priv_key = "a826c4636474b208605393fdc4f664332ef8a3594474909f5e093738d893cc56431d223b1cd403d725e98058bbff9d02",
    //QeCAVSx = 81a5f71f204c3f4a96fe5aea9ceecfbedea8accf18925465bcb87b5fdb566079d4c909cb900efd2a7e6891bf91cc3afb
    //QeCAVSy = 82ae0a4b3b94445a5658326805a0501787f34249eb0ec63f1b06d0e05bc72e7888654f94af49cb425a8c1927bc4ad741
    .eph_priv_key = "00eb5222e7e73c887e45df9cf54b10e53a6a1ed8ab98a8363cd35fb0cdc5c05dee0642c71addaee88be7e81385f14060",
    //QsIUTx = 55c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d38
    //QsIUTy = 84b6258ec0079c4ac4e2efcd848ffc74fb2972a0228aed059549e04a9ab021dddf6d6fee19d7e5e4406bc49ad1023c20
    //Z = deb56295d036ce1abe61070aba1435102665b911b64188a8ed9778c2ecf1606d51416d9370f5665f8525694753416e26
    //KDF(Z,00000001,"Test1") = 4ebb4a7bd3be801f115083fa4991ba60
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = d2e5afa3c841d3d51e9f0db500
    // Mac Tag = bc2f1d62dd61281fc9b020c7e7ac2147
    .cipher = "0455c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d3884b6258ec0079c4ac4e2efcd848ffc74fb2972a0228aed059549e04a9ab021dddf6d6fee19d7e5e4406bc49ad1023c20d2e5afa3c841d3d51e9f0db500bc2f1d62dd61281fc9b020c7e7ac2147",
    .compact_cipher = "55c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d38d2e5afa3c841d3d51e9f0db500bc2f1d62dd61281fc9b020c7e7ac2147",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="Test1",
    .sharedInfo2="Test2",
},
// P384, SHA384, diversification SharedInfo 1 = Ephemeral public point
{
    .di=di_SHA384,
    .curve=&ccec_cp_384,
    .mac_length = 12,
    .key_length = 32,
    .dec_priv_key = "a826c4636474b208605393fdc4f664332ef8a3594474909f5e093738d893cc56431d223b1cd403d725e98058bbff9d02",
    //QeCAVSx = 81a5f71f204c3f4a96fe5aea9ceecfbedea8accf18925465bcb87b5fdb566079d4c909cb900efd2a7e6891bf91cc3afb
    //QeCAVSy = 82ae0a4b3b94445a5658326805a0501787f34249eb0ec63f1b06d0e05bc72e7888654f94af49cb425a8c1927bc4ad741
    .eph_priv_key = "00eb5222e7e73c887e45df9cf54b10e53a6a1ed8ab98a8363cd35fb0cdc5c05dee0642c71addaee88be7e81385f14060",
    //QsIUTx = 55c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d38
    //QsIUTy = 84b6258ec0079c4ac4e2efcd848ffc74fb2972a0228aed059549e04a9ab021dddf6d6fee19d7e5e4406bc49ad1023c20
    //Z = deb56295d036ce1abe61070aba1435102665b911b64188a8ed9778c2ecf1606d51416d9370f5665f8525694753416e26
    //KDF(Z,0455c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d3884b6258ec0079c4ac4e2efcd848ffc74fb2972a0228aed059549e04a9ab021dddf6d6fee19d7e5e4406bc49ad1023c20
    //    ) = 1324ab00fa742d67d16a497c3ac6e537befebb82cc248f5db9e40e47405bbbf5
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = 60e3b614c7694da3000004001d
    // Mac Tag = 664e1020ebce49cc0fb3ed5b
    .cipher = "0455c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d3884b6258ec0079c4ac4e2efcd848ffc74fb2972a0228aed059549e04a9ab021dddf6d6fee19d7e5e4406bc49ad1023c2060e3b614c7694da3000004001d664e1020ebce49cc0fb3ed5b",
    .compact_cipher =   "55c99300151be1c755058a125eb027ac188655bb7c002004541c74df24543066e7921e26f64d916476adddee04737d38f3d7dbc92e88e06d8155c8439e182920ca973c29903b451786",
    .options=ECIES_EXPORT_PUB_STANDARD+ECIES_EPH_PUBKEY_IN_SHAREDINFO1,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P521, SHA512, no diversification
{
    .di=di_SHA512,
    .curve=&ccec_cp_521,
    .mac_length = 16,
    .key_length = 16,
    .dec_priv_key = "000000ccf9c2481d36c1a7a9ad516a63e60871c43e74fe6392409ac5e939dff933df5b6edfcae04966ee5e8da8f7c3232572a084a84b2a1db370d93729b70ebd822cc35a",
    //QeCAVSx = 000001e12f3e23bdd037d7e607cdc7253c99434b290189b7ebe29c04e568aae2d43aee68449f7bc59363cfa8d295d712428af93113eb7c6a50444a0881725997200c36f3
    //QeCAVSy = 000000bdb19d1c7a9aee4819e1a1a4358c285242acdba2bd4edf627c905ba33e33557cd2f12cbf444cef49ba679ab5c7e8590bf05969c5fb52df6d5b88e8984ea261185a
    .eph_priv_key = "000000ea3f82bf7c3d8a762a5b8ef1031d4c477ed9023a2a4671139b58066fbb393343fd68451c768a6ac8e5756409fd6fce0064aa27f8b345831f80e3ff9360171f682d",
    //QsIUTx = 000000d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae57
    //QsIUTy = 000000880a741d94a068e9b8e9b979bdcccc5ccf10fc768115d2db635b531e84374d4872d423f6fd43fca25f3c881387fb19f4376e328024341c8ecac77335748c90e76b
    //Z = 005b63293248ef5ebe1852be8f6bdcc8a679b293928c3586fc53dc7c1cf69fa760987a40044f7dba38f8c08e2566e24e7b8e9dc3080b95fa3dbe549eb1aaa0665d4b
    //KDF(Z,00000001,"") = 140741425a7e0c2e8b96df45fe64ca08
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = f82e8912427a40c0937e4f0164
    // Mac Tag = 3d7cb583c336c66504c6a05278850df3
    .cipher = "0400d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae5700880a741d94a068e9b8e9b979bdcccc5ccf10fc768115d2db635b531e84374d4872d423f6fd43fca25f3c881387fb19f4376e328024341c8ecac77335748c90e76bf82e8912427a40c0937e4f01643d7cb583c336c66504c6a05278850df3",
    .compact_cipher = "00d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae57f82e8912427a40c0937e4f01643d7cb583c336c66504c6a05278850df3",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="",
    .sharedInfo2="",
},
// P521, SHA512, diversification SharedInfo 1 & 2
{
    .di=di_SHA512,
    .curve=&ccec_cp_521,
    .mac_length = 16,
    .key_length = 24,
    .dec_priv_key = "000000ccf9c2481d36c1a7a9ad516a63e60871c43e74fe6392409ac5e939dff933df5b6edfcae04966ee5e8da8f7c3232572a084a84b2a1db370d93729b70ebd822cc35a",
    //QeCAVSx = 000001e12f3e23bdd037d7e607cdc7253c99434b290189b7ebe29c04e568aae2d43aee68449f7bc59363cfa8d295d712428af93113eb7c6a50444a0881725997200c36f3
    //QeCAVSy = 000000bdb19d1c7a9aee4819e1a1a4358c285242acdba2bd4edf627c905ba33e33557cd2f12cbf444cef49ba679ab5c7e8590bf05969c5fb52df6d5b88e8984ea261185a
    .eph_priv_key = "000000ea3f82bf7c3d8a762a5b8ef1031d4c477ed9023a2a4671139b58066fbb393343fd68451c768a6ac8e5756409fd6fce0064aa27f8b345831f80e3ff9360171f682d",
    //QsIUTx = 000000d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae57
    //QsIUTy = 000000880a741d94a068e9b8e9b979bdcccc5ccf10fc768115d2db635b531e84374d4872d423f6fd43fca25f3c881387fb19f4376e328024341c8ecac77335748c90e76b
    //Z = 005b63293248ef5ebe1852be8f6bdcc8a679b293928c3586fc53dc7c1cf69fa760987a40044f7dba38f8c08e2566e24e7b8e9dc3080b95fa3dbe549eb1aaa0665d4b
    //KDF(Z,00000001,"Test1") = baaf86745769c7291cf6f88df9800972263f59d6246e5429
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = cc9bddc0f1d8e8fc56365fcd8b
    // Mac Tag = a0db1a86da5de38d8ecd878840e55191
    .cipher = "0400d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae5700880a741d94a068e9b8e9b979bdcccc5ccf10fc768115d2db635b531e84374d4872d423f6fd43fca25f3c881387fb19f4376e328024341c8ecac77335748c90e76bcc9bddc0f1d8e8fc56365fcd8ba0db1a86da5de38d8ecd878840e55191",
    .compact_cipher =
"00d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae57cc9bddc0f1d8e8fc56365fcd8ba0db1a86da5de38d8ecd878840e55191",
    .options=ECIES_EXPORT_PUB_STANDARD,
    .sharedInfo1="Test1",
    .sharedInfo2="Test2",
},
// P521, SHA512, diversification SharedInfo 1 = Ephemeral public point
{
    .di=di_SHA512,
    .curve=&ccec_cp_521,
    .mac_length = 12,
    .key_length = 32,
    .dec_priv_key = "000000ccf9c2481d36c1a7a9ad516a63e60871c43e74fe6392409ac5e939dff933df5b6edfcae04966ee5e8da8f7c3232572a084a84b2a1db370d93729b70ebd822cc35a",
    //QeCAVSx = 000001e12f3e23bdd037d7e607cdc7253c99434b290189b7ebe29c04e568aae2d43aee68449f7bc59363cfa8d295d712428af93113eb7c6a50444a0881725997200c36f3
    //QeCAVSy = 000000bdb19d1c7a9aee4819e1a1a4358c285242acdba2bd4edf627c905ba33e33557cd2f12cbf444cef49ba679ab5c7e8590bf05969c5fb52df6d5b88e8984ea261185a
    .eph_priv_key = "000000ea3f82bf7c3d8a762a5b8ef1031d4c477ed9023a2a4671139b58066fbb393343fd68451c768a6ac8e5756409fd6fce0064aa27f8b345831f80e3ff9360171f682d",
    //QsIUTx = 000000d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae57
    //QsIUTy = 000000880a741d94a068e9b8e9b979bdcccc5ccf10fc768115d2db635b531e84374d4872d423f6fd43fca25f3c881387fb19f4376e328024341c8ecac77335748c90e76b
    //Z = 005b63293248ef5ebe1852be8f6bdcc8a679b293928c3586fc53dc7c1cf69fa760987a40044f7dba38f8c08e2566e24e7b8e9dc3080b95fa3dbe549eb1aaa0665d4b
    //KDF(Z,0400d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae5700880a741d94a068e9b8e9b979bdcccc5ccf10fc768115d2db635b531e84374d4872d423f6fd43fca25f3c881387fb19f4376e328024341c8ecac77335748c90e76b
    //    ) = d4c84d7a57a957c689160b153e98041e3f81e4af9b6d72cdc877448f5e1424a5
    .message = "f04e8860e6b86552cfa933c387",
    // Encrypted message = 746bbec57c2607b3b5bd98cae3
    // Mac Tag = e659ab496e01e32ddf58a126
    .cipher = "0400d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae5700880a741d94a068e9b8e9b979bdcccc5ccf10fc768115d2db635b531e84374d4872d423f6fd43fca25f3c881387fb19f4376e328024341c8ecac77335748c90e76b746bbec57c2607b3b5bd98cae3e659ab496e01e32ddf58a126",
    .compact_cipher ="00d5baed4fd07eb677c3ceaf14543a4797305a68c3235ff1df538da35a9b4eb0a7c4392640b87b2cf9924e525290535e17d6e3517b5a21486bdea96afe3d3f8eae575962cf5c7002c6c00f09473846e6a9e8418c379954260aebd6",

    .options=ECIES_EXPORT_PUB_STANDARD+ECIES_EPH_PUBKEY_IN_SHAREDINFO1,
    .sharedInfo1="",
    .sharedInfo2="",
},
{
    .di=NULL,
}
