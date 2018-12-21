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
		"rect" : [ 315.0, 79.0, 624.0, 682.0 ],
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
					"args" : [ "@module", 2 ],
					"bgmode" : 0,
					"border" : 0,
					"clickthrough" : 0,
					"enablehscroll" : 0,
					"enablevscroll" : 0,
					"id" : "obj-13",
					"lockeddragscroll" : 0,
					"maxclass" : "bpatcher",
					"name" : "demosound.maxpat",
					"numinlets" : 0,
					"numoutlets" : 1,
					"offset" : [ 0.0, 0.0 ],
					"outlettype" : [ "signal" ],
					"patching_rect" : [ 52.0, 243.5, 230.0, 95.0 ],
					"viewvisibility" : 1
				}

			}
, 			{
				"box" : 				{
					"border" : 0,
					"filename" : "helpdetails.js",
					"id" : "obj-4",
					"ignoreclick" : 1,
					"jsarguments" : [ "GoMoDo" ],
					"maxclass" : "jsui",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 9.0, 10.0, 606.0, 222.0 ]
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 291.5, 604.0, 83.0, 22.0 ],
					"text" : "print GoMoDo"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-35",
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 52.0, 604.0, 45.0, 45.0 ]
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
					"patching_rect" : [ 52.0, 374.0, 258.5, 196.0 ],
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
									"BufferState" : 0.0,
									"Threshold" : -24.768153980752416,
									"ThresholdMode" : 0.0,
									"RandomizeDurations" : 0.0,
									"RandomizeGains" : 0.0,
									"RandomizePeriods" : 0.0,
									"RandomizePitchShifts" : 0.0,
									"BufferFlux" : 50.0,
									"Duration1" : 25.0,
									"Duration2" : 50.0,
									"Gain1" : 0.0,
									"Gain2" : 0.0,
									"Period1" : 30.0,
									"Period2" : 30.0,
									"PitchShift1" : 0.0,
									"PitchShift2" : 0.0,
									"Dry/Wet" : 50.0
								}

							}

						}
,
						"snapshotlist" : 						{
							"current_snapshot" : 1,
							"entries" : [ 								{
									"filetype" : "C74Snapshot",
									"version" : 2,
									"minorversion" : 0,
									"name" : "default",
									"origin" : "NewGrain.amxd",
									"type" : "amxd",
									"subtype" : "Undefined",
									"embed" : 0,
									"snapshot" : 									{
										"name" : "NewGrain.amxd",
										"origname" : "/Storage/documents/Max/Packages/LowkeyNW/patchers/newgrain/NewGrain.amxd",
										"valuedictionary" : 										{
											"parameter_values" : 											{
												"BufferState" : 0.0,
												"Threshold" : -24.768153980752416,
												"ThresholdMode" : 0.0,
												"RandomizeDurations" : 0.0,
												"RandomizeGains" : 0.0,
												"RandomizePeriods" : 0.0,
												"RandomizePitchShifts" : 0.0,
												"BufferFlux" : 50.0,
												"Duration1" : 25.0,
												"Duration2" : 50.0,
												"Gain1" : 0.0,
												"Gain2" : 0.0,
												"Period1" : 30.0,
												"Period2" : 30.0,
												"PitchShift1" : 0.0,
												"PitchShift2" : 0.0,
												"Dry/Wet" : 50.0
											}

										}

									}
,
									"fileref" : 									{
										"name" : "NewGrain.amxd[1]",
										"filename" : "NewGrain.amxd[1].maxsnap",
										"filepath" : "/Storage/documents/Max 8/Snapshots",
										"filepos" : -1,
										"snapshotfileid" : "00967a67dbce4b1b2499bc4d7e5fe9ef"
									}

								}
, 								{
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
												"BufferState" : 0.0,
												"Threshold" : -24.768153980752416,
												"ThresholdMode" : 0.0,
												"RandomizeDurations" : 0.0,
												"RandomizeGains" : 0.0,
												"RandomizePeriods" : 0.0,
												"RandomizePitchShifts" : 0.0,
												"BufferFlux" : 50.0,
												"Duration1" : 25.0,
												"Duration2" : 50.0,
												"Gain1" : 0.0,
												"Gain2" : 0.0,
												"Period1" : 30.0,
												"Period2" : 30.0,
												"PitchShift1" : 0.0,
												"PitchShift2" : 0.0,
												"Dry/Wet" : 50.0
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
					"patching_rect" : [ 305.0, 320.0, 65.0, 22.0 ],
					"text" : "getparams"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 1 ],
					"order" : 0,
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"order" : 1,
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
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
					"destination" : [ "obj-2", 0 ],
					"source" : [ "obj-5", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-13::obj-21::obj-6" : [ "live.tab[3]", "live.tab[1]", 0 ],
			"obj-2" : [ "amxd~", "amxd~", 0 ],
			"obj-13::obj-35" : [ "[5]", "Level", 0 ],
			"parameterbanks" : 			{

			}

		}
,
		"dependency_cache" : [ 			{
				"name" : "NewGrain.amxd[1].maxsnap",
				"bootpath" : "/Storage/documents/Max 8/Snapshots",
				"patcherrelativepath" : "../../../../Max 8/Snapshots",
				"type" : "mx@s",
				"implicit" : 1
			}
, 			{
				"name" : "NewGrain.amxd.maxsnap",
				"bootpath" : "/Storage/documents/Max 8/Snapshots",
				"patcherrelativepath" : "../../../../Max 8/Snapshots",
				"type" : "mx@s",
				"implicit" : 1
			}
, 			{
				"name" : "helpdetails.js",
				"bootpath" : "C74:/help/resources",
				"type" : "TEXT",
				"implicit" : 1
			}
, 			{
				"name" : "demosound.maxpat",
				"bootpath" : "C74:/help/msp",
				"type" : "JSON",
				"implicit" : 1
			}
, 			{
				"name" : "sine.svg",
				"bootpath" : "C74:/media/max/picts/m4l-picts",
				"type" : "svg",
				"implicit" : 1
			}
, 			{
				"name" : "saw.svg",
				"bootpath" : "C74:/media/max/picts/m4l-picts",
				"type" : "svg",
				"implicit" : 1
			}
, 			{
				"name" : "square.svg",
				"bootpath" : "C74:/media/max/picts/m4l-picts",
				"type" : "svg",
				"implicit" : 1
			}
, 			{
				"name" : "random.svg",
				"bootpath" : "C74:/media/max/picts/m4l-picts",
				"type" : "svg",
				"implicit" : 1
			}
, 			{
				"name" : "interfacecolor.js",
				"bootpath" : "C74:/interfaces",
				"type" : "TEXT",
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
