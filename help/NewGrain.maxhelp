{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 0,
			"revision" : 2,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 212.0, 193.0, 640.0, 480.0 ],
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
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 357.0, 400.0, 32.0, 22.0 ],
					"text" : "print"
				}

			}
, 			{
				"box" : 				{
					"data" : 					{
						"clips" : [ 							{
								"absolutepath" : "StetsonMac:/Users/lowkeynw/Desktop/NewGrainDev/1008chaz.m4a",
								"filename" : "1008chaz.m4a",
								"filekind" : "audiofile",
								"loop" : 1,
								"content_state" : 								{
									"play" : [ 0 ],
									"pitchshift" : [ 1.0 ],
									"originaltempo" : [ 120.0 ],
									"basictuning" : [ 440 ],
									"mode" : [ "basic" ],
									"speed" : [ 1.0 ],
									"quality" : [ "basic" ],
									"slurtime" : [ 0.0 ],
									"pitchcorrection" : [ 0 ],
									"followglobaltempo" : [ 0 ],
									"timestretch" : [ 0 ],
									"originallengthms" : [ 0.0 ],
									"formantcorrection" : [ 0 ],
									"formant" : [ 1.0 ],
									"originallength" : [ 0.0, "ticks" ]
								}

							}
 ]
					}
,
					"id" : "obj-3",
					"maxclass" : "playlist~",
					"numinlets" : 1,
					"numoutlets" : 5,
					"outlettype" : [ "signal", "signal", "signal", "", "dictionary" ],
					"patching_rect" : [ 73.0, 26.0, 150.0, 30.0 ]
				}

			}
, 			{
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
					"patching_rect" : [ 101.0, 165.0, 258.5, 196.0 ],
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
									"BufferFlux" : 86.614173228346417,
									"BufferState" : 0.0,
									"Dry/Wet" : 59.055118110236258,
									"Duration1" : 82.677165354330725,
									"Duration2" : 149.606299212598429,
									"Gain1" : 5.669291338582674,
									"Gain2" : 0.0,
									"OverThreshold" : 0.0,
									"Period1" : 43.30708661417323,
									"Period2" : 94.488188976378069,
									"PitchShift1" : 18.0,
									"PitchShift2" : -24.0,
									"RandomDurations" : 1.0,
									"RandomGains" : 0.0,
									"RandomPeriods" : 0.0,
									"RandomPitchShifts" : 1.0,
									"Threshold" : -80.0,
									"ThresholdMode" : 0.0,
									"live.text" : 0.0
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
												"BufferFlux" : 86.614173228346417,
												"BufferState" : 0.0,
												"Dry/Wet" : 59.055118110236258,
												"Duration1" : 82.677165354330725,
												"Duration2" : 149.606299212598429,
												"Gain1" : 5.669291338582674,
												"Gain2" : 0.0,
												"OverThreshold" : 0.0,
												"Period1" : 43.30708661417323,
												"Period2" : 94.488188976378069,
												"PitchShift1" : 18.0,
												"PitchShift2" : -24.0,
												"RandomDurations" : 1.0,
												"RandomGains" : 0.0,
												"RandomPeriods" : 0.0,
												"RandomPitchShifts" : 1.0,
												"Threshold" : -80.0,
												"ThresholdMode" : 0.0,
												"live.text" : 0.0
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
, 			{
				"box" : 				{
					"id" : "obj-5",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 119.0, 125.0, 65.0, 22.0 ],
					"text" : "getparams"
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
					"destination" : [ "obj-6", 0 ],
					"source" : [ "obj-2", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 1 ],
					"source" : [ "obj-3", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"source" : [ "obj-5", 0 ]
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
				"name" : "1008chaz.m4a",
				"bootpath" : "~/Desktop/NewGrainDev",
				"patcherrelativepath" : "../../../../../../Users/lowkeynw/Desktop/NewGrainDev",
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
