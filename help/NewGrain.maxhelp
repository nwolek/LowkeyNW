{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 0,
			"revision" : 0,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 59.0, 104.0, 640.0, 480.0 ],
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
					"id" : "obj-35",
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 101.0, 395.0, 45.0, 45.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-77",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 185.0, 73.0, 203.0, 20.0 ],
					"text" : "for testing, remove from final version"
				}

			}
, 			{
				"box" : 				{
					"data" : 					{
						"clips" : [ 							{
								"filename" : "StetsonMac:/Users/lowkeynw/Desktop/chaz1008.m4a",
								"filekind" : "audiofile",
								"loop" : 1,
								"content_state" : 								{
									"pitchcorrection" : [ 0 ],
									"formant" : [ 1.0 ],
									"followglobaltempo" : [ 0 ],
									"mode" : [ "basic" ],
									"timestretch" : [ 0 ],
									"play" : [ 0 ],
									"quality" : [ "basic" ],
									"pitchshift" : [ 1.0 ],
									"formantcorrection" : [ 0 ],
									"originallength" : [ 0.0, "ticks" ],
									"speed" : [ 1.0 ],
									"slurtime" : [ 0.0 ],
									"originaltempo" : [ 120.0 ],
									"basictuning" : [ 440 ],
									"originallengthms" : [ 0.0 ]
								}

							}
 ]
					}
,
					"id" : "obj-95",
					"maxclass" : "playlist~",
					"numinlets" : 1,
					"numoutlets" : 5,
					"outlettype" : [ "signal", "signal", "signal", "", "dictionary" ],
					"patching_rect" : [ 185.0, 93.0, 150.0, 30.0 ]
				}

			}
, 			{
				"box" : 				{
					"autosave" : 1,
					"bgmode" : 1,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-2",
					"lockeddragscroll" : 0,
					"maxclass" : "newobj",
					"numinlets" : 3,
					"numoutlets" : 4,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal", "signal", "", "" ],
					"patching_rect" : [ 101.0, 165.0, 252.5, 196.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_invisible" : 1,
							"parameter_longname" : "amxd~",
							"parameter_shortname" : "amxd~",
							"parameter_type" : 3
						}

					}
,
					"saved_object_attributes" : 					{
						"parameter_enable" : 1,
						"patchername" : "NewGrain.amxd",
						"patchername_fallback" : "/Storage/documents/Max/Packages/LowkeyNW/patchers/newgrain/NewGrain.amxd"
					}
,
					"snapshot" : 					{
						"filetype" : "C74Snapshot",
						"version" : 2,
						"minorversion" : 0,
						"name" : "snapshotlist",
						"origin" : "max~",
						"type" : "list",
						"subtype" : "Undefined",
						"embed" : 1,
						"snapshot" : 						{
							"name" : "NewGrain.amxd",
							"origname" : "/Storage/documents/Max/Packages/LowkeyNW/patchers/newgrain/NewGrain.amxd",
							"valuedictionary" : 							{
								"parameter_values" : 								{
									"BufferFlux" : 47.244094488188978,
									"Dry/Wet" : 64.566929133858224,
									"Duration1" : 39.370078740157481,
									"Gain" : 0.0,
									"Period1" : 19.685039370078741,
									"Period2" : 51.181102362204726,
									"PitchShift" : 0.0,
									"live.text" : 0.0,
									"live.text[7]" : 0.0,
									"Duration2" : 145.66929133858261,
									"GainFlux" : 0.0,
									"PitchFlux" : 0.0
								}

							}

						}
,
						"snapshotlist" : 						{
							"current_snapshot" : 0,
							"entries" : [ 								{
									"filetype" : "C74Snapshot",
									"version" : 2,
									"minorversion" : 0,
									"name" : "NewGrain.amxd",
									"origin" : "NewGrain.amxd",
									"type" : "amxd",
									"subtype" : "Undefined",
									"embed" : 0,
									"snapshot" : 									{
										"name" : "NewGrain.amxd",
										"origname" : "/Storage/documents/Max/Packages/LowkeyNW/patchers/newgrain/NewGrain.amxd",
										"valuedictionary" : 										{
											"parameter_values" : 											{
												"BufferFlux" : 47.244094488188978,
												"Dry/Wet" : 64.566929133858224,
												"Duration1" : 39.370078740157481,
												"Gain" : 0.0,
												"Period1" : 19.685039370078741,
												"Period2" : 51.181102362204726,
												"PitchShift" : 0.0,
												"live.text" : 0.0,
												"live.text[7]" : 0.0,
												"Duration2" : 145.66929133858261,
												"GainFlux" : 0.0,
												"PitchFlux" : 0.0
											}

										}

									}
,
									"fileref" : 									{
										"name" : "NewGrain.amxd",
										"filename" : "NewGrain.amxd.maxsnap",
										"filepath" : "/Storage/documents/Max 8/Snapshots",
										"filepos" : -1,
										"snapshotfileid" : "7c48d12dbf3e8ef3cf14e47cbcecb3a6"
									}

								}
 ]
						}

					}
,
					"text" : "amxd~ NewGrain.amxd",
					"varname" : "amxd~",
					"viewvisibility" : 1
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-35", 1 ],
					"source" : [ "obj-2", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-35", 0 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 1 ],
					"source" : [ "obj-95", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"source" : [ "obj-95", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-2" : [ "amxd~", "amxd~", 0 ],
			"parameterbanks" : 			{

			}

		}
,
		"dependency_cache" : [ 			{
				"name" : "NewGrain.amxd.maxsnap",
				"bootpath" : "/Storage/documents/Max 8/Snapshots",
				"patcherrelativepath" : "../../../../Max 8/Snapshots",
				"type" : "mx@s",
				"implicit" : 1
			}
, 			{
				"name" : "NewGrain.amxd",
				"bootpath" : "/Storage/documents/Max/Packages/LowkeyNW/patchers/newgrain",
				"patcherrelativepath" : "../patchers/newgrain",
				"type" : "amxd",
				"implicit" : 1
			}
, 			{
				"name" : "nw.randrange~.maxpat",
				"bootpath" : "/Storage/documents/Max/Packages/LowkeyNW/patchers/newgrain",
				"patcherrelativepath" : "../patchers/newgrain",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "chaz1008.m4a",
				"bootpath" : "~/Desktop",
				"patcherrelativepath" : "../../../../../../Users/lowkeynw/Desktop",
				"type" : "M4a",
				"implicit" : 1
			}
, 			{
				"name" : "nw.grainpulse~.mxo",
				"type" : "iLaX"
			}
, 			{
				"name" : "nw.recordplus~.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0
	}

}
