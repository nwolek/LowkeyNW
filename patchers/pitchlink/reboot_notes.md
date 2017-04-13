# Musiclinks reboot

## Next steps on 28 March 2017
1. memory / query system so that new tracks get current settings from network - DONE
2. better way to display scale struct -- is that a kslider? YES
3. Right now Live Device overwrites pitch data. Is there a more elegant solution?
	random, arp (up, down, up-down), one pitch
4. Master control is sent from Max. Is this OK?
	needs a pattern changing option
5. Pitch patterns 
	random, arp (up, down, up-down)
6. durations
	- limit to "n" only

## questions on 4 April 2017
1. what if it was called "pitchlinks" to be clearer? - DONE
2. stickiness - should it be based on onsets received or tied to rhythmic loop? onsets preferred by NW - onsets DONE
3. pitch information should be moved to dict - DONE

## reaction from 7 April [social media demo](https://www.youtube.com/watch?v=l_Ya9nHSqVA)
1. ideally midi duration would be [preserved via Borax](https://docs.cycling74.com/max5/refpages/max-ref/borax.html) - DONE
2. would be great to tie in with key info on Push. Need to review [documentation from Ableton](https://github.com/Ableton/push-interface) 
3. NW wants to export and import settings for caller - DONE
4. saving length of arrays is error prone, so remove