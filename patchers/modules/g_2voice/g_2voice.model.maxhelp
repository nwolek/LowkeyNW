{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 7,
			"minor" : 0,
			"revision" : 3,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"rect" : [ 255.0, 189.0, 699.0, 643.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"boxes" : [ 			{
				"box" : 				{
					"border" : 0,
					"filename" : "helpdetails.js",
					"id" : "obj-21",
					"ignoreclick" : 1,
					"jsarguments" : [ "g_voice2.model" ],
					"maxclass" : "jsui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 6.0, 5.0, 679.0, 69.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-12",
					"maxclass" : "live.gain~",
					"numinlets" : 2,
					"numoutlets" : 5,
					"orientation" : 1,
					"outlettype" : [ "signal", "signal", "", "float", "list" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 78.0, 442.0, 165.0, 48.0 ],
					"presentation_rect" : [ 15.0, 15.0, 50.0, 48.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_longname" : "Gain",
							"parameter_shortname" : "Gain",
							"parameter_type" : 0,
							"parameter_mmin" : -70.0,
							"parameter_mmax" : 6.0,
							"parameter_initial" : [ 0.0 ],
							"parameter_unitstyle" : 4
						}

					}
,
					"varname" : "live.gain~"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-13",
					"local" : 1,
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 78.0, 511.0, 45.0, 45.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"bubble" : 1,
					"fontname" : "Arial",
					"fontsize" : 13.0,
					"id" : "obj-26",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 280.0, 148.0, 103.0, 25.0 ],
					"style" : "",
					"text" : "Play a sound"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 1.0, 0.788235, 0.470588, 1.0 ],
					"fontname" : "Arial Bold",
					"hint" : "",
					"id" : "obj-27",
					"ignoreclick" : 1,
					"legacytextcolor" : 1,
					"maxclass" : "textbutton",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 384.0, 148.0, 20.0, 20.0 ],
					"rounded" : 60.0,
					"style" : "",
					"text" : "N",
					"textcolor" : [ 0.34902, 0.34902, 0.34902, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"clipheight" : 34.5,
					"data" : 					{
						"clips" : [ 							{
								"filename" : "cello-f2.aif",
								"filekind" : "audiofile",
								"loop" : 0,
								"content_state" : 								{
									"quality" : [ "basic" ],
									"originaltempo" : [ 120.0 ],
									"pitchcorrection" : [ 0 ],
									"timestretch" : [ 0 ],
									"originallength" : [ 0.0, "ticks" ],
									"slurtime" : [ 0.0 ],
									"play" : [ 0 ],
									"formantcorrection" : [ 0 ],
									"mode" : [ "basic" ],
									"pitchshift" : [ 1.0 ],
									"formant" : [ 1.0 ],
									"basictuning" : [ 440 ],
									"followglobaltempo" : [ 0 ],
									"speed" : [ 1.0 ],
									"originallengthms" : [ 0.0 ],
									"pitchshiftcent" : [ 0 ]
								}

							}
, 							{
								"filename" : "duduk.aif",
								"filekind" : "audiofile",
								"loop" : 1,
								"content_state" : 								{
									"quality" : [ "basic" ],
									"originaltempo" : [ 120.0 ],
									"pitchcorrection" : [ 0 ],
									"timestretch" : [ 0 ],
									"originallength" : [ 0.0, "ticks" ],
									"slurtime" : [ 0.0 ],
									"play" : [ 0 ],
									"formantcorrection" : [ 0 ],
									"mode" : [ "basic" ],
									"pitchshift" : [ 1.0 ],
									"formant" : [ 1.0 ],
									"basictuning" : [ 440 ],
									"followglobaltempo" : [ 0 ],
									"speed" : [ 1.0 ],
									"originallengthms" : [ 0.0 ],
									"pitchshiftcent" : [ 0 ]
								}

							}
, 							{
								"filename" : "sho0630.aif",
								"filekind" : "audiofile",
								"loop" : 0,
								"content_state" : 								{
									"quality" : [ "basic" ],
									"originaltempo" : [ 120.0 ],
									"pitchcorrection" : [ 0 ],
									"timestretch" : [ 0 ],
									"originallength" : [ 0.0, "ticks" ],
									"slurtime" : [ 0.0 ],
									"play" : [ 0 ],
									"formantcorrection" : [ 0 ],
									"mode" : [ "basic" ],
									"pitchshift" : [ 1.0 ],
									"formant" : [ 1.0 ],
									"basictuning" : [ 440 ],
									"followglobaltempo" : [ 0 ],
									"speed" : [ 1.0 ],
									"originallengthms" : [ 0.0 ],
									"pitchshiftcent" : [ 0 ]
								}

							}
, 							{
								"filename" : "vibes-a1.aif",
								"filekind" : "audiofile",
								"loop" : 0,
								"content_state" : 								{
									"quality" : [ "basic" ],
									"originaltempo" : [ 120.0 ],
									"pitchcorrection" : [ 0 ],
									"timestretch" : [ 0 ],
									"originallength" : [ 0.0, "ticks" ],
									"slurtime" : [ 0.0 ],
									"play" : [ 0 ],
									"formantcorrection" : [ 0 ],
									"mode" : [ "basic" ],
									"pitchshift" : [ 1.0 ],
									"formant" : [ 1.0 ],
									"basictuning" : [ 440 ],
									"followglobaltempo" : [ 0 ],
									"speed" : [ 1.0 ],
									"originallengthms" : [ 0.0 ],
									"pitchshiftcent" : [ 0 ]
								}

							}
 ]
					}
,
					"id" : "obj-11",
					"maxclass" : "playlist~",
					"numinlets" : 1,
					"numoutlets" : 5,
					"outlettype" : [ "signal", "signal", "signal", "", "dictionary" ],
					"patching_rect" : [ 78.0, 148.0, 200.0, 142.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"bgmode" : 0,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-3",
					"lockeddragscroll" : 0,
					"maxclass" : "bpatcher",
					"name" : "g_2voice.module.maxpat",
					"numinlets" : 2,
					"numoutlets" : 3,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal", "signal", "" ],
					"patching_rect" : [ 78.0, 324.0, 300.0, 70.0 ],
					"presentation_rect" : [ 0.0, 0.0, 300.0, 70.0 ],
					"viewvisibility" : 1
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 1 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-11", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-11", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 1 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-12", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-13", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 1 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-3", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-12", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-3", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-3::obj-1::obj-9" : [ "live.numbox", "live.numbox", 0 ],
			"obj-3::obj-1::obj-16" : [ "live.numbox[2]", "live.numbox", 0 ],
			"obj-3::obj-1::obj-8" : [ "live.toggle", "live.toggle", 0 ],
			"obj-3::obj-1::obj-11" : [ "live.numbox[1]", "live.numbox", 0 ],
			"obj-12" : [ "Gain", "Gain", 0 ]
		}
,
		"dependency_cache" : [ 			{
				"name" : "g_2voice.module.maxpat",
				"bootpath" : "/Volumes/Storage/Documents/Max/Packages/LowkeyNW/patchers/modules/g_2voice",
				"patcherrelativepath" : ".",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "g_2voice.model.maxpat",
				"bootpath" : "/Volumes/Storage/Documents/Max/Packages/LowkeyNW/patchers/modules/g_2voice",
				"patcherrelativepath" : ".",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "g_2voice.view.maxpat",
				"bootpath" : "/Volumes/Storage/Documents/Max/Packages/LowkeyNW/patchers/modules/g_2voice",
				"patcherrelativepath" : ".",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "helpdetails.js",
				"bootpath" : "C74:/help/resources",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "j.in~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.out~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.model.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "nw.trainshift~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "nw.recordplus~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "nw.grainpulse~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.message.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.parameter.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.ui.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.view.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.receive~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "j.remote.mxo",
				"type" : "iLaX"
			}
 ],
		"embedsnapshot" : 0
	}

}
