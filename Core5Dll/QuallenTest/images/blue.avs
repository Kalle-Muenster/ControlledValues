blanko = BlankClip( width=156, height=172, fps=18, audio_rate=44100 )
anfang = AudioDub( blanko.Animate( 0, 48, "Subtitle", "Hallo Qualle", 100, 120, 0, 99999, "Arial", 0, "Hallo Qualle", 25, 130, 0, 99999, "Arial", 48 ), blanko )
qualle = AudioDub( ImageSource( "Q2_%i.png", start=0, end=23, fps=18, pixel_type="rgb32" ), blanko ).repeatClip( times=20 )
splash = AudioDub( ImageSource( "Q2_%i.png", start=24, end=35, fps=18, pixel_type="rgb32" ), WAVSource( "..\audios\mono-blub.wav" ) ) 
return anfang ++ qualle ++ splash
