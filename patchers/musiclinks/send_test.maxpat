{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 7,
			"minor" : 2,
			"revision" : 4,
			"architecture" : "x86",
			"modernui" : 1
		}
,
		"rect" : [ 283.0, 192.0, 640.0, 480.0 ],
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
					"id" : "obj-8",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 4,
					"outlettype" : [ "dictionary", "", "", "" ],
					"patching_rect" : [ 437.0, 391.0, 50.5, 22.0 ],
					"saved_object_attributes" : 					{
						"embed" : 0,
						"parameter_enable" : 0
					}
,
					"style" : "",
					"text" : "dict"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-5",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 470.0, 247.0, 122.0, 22.0 ],
					"style" : "",
					"text" : "prepend scale_basis"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 18.0,
					"id" : "obj-6",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 87.83252, 150.0, 59.0, 27.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 87.83252, 148.5, 59.0, 27.0 ],
					"style" : "",
					"text" : "tonic:"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-41",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 238.0, 197.312271, 161.0, 22.0 ],
					"style" : "",
					"text" : "clearchecks, checkitem $1 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 18.0,
					"id" : "obj-75",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 223.83252, 150.0, 59.0, 27.0 ],
					"presentation" : 1,
					"presentation_rect" : [ 74.0, 240.0, 59.0, 27.0 ],
					"style" : "",
					"text" : "basis:"
				}

			}
, 			{
				"box" : 				{
					"allowdrag" : 0,
					"fontsize" : 18.0,
					"id" : "obj-151",
					"items" : [ "Ionian", "(Major)", ",", "Dorian", ",", "Phrygian", ",", "Lydian", ",", "Mixolydian", ",", "Aeolian", "(Natural", "Minor)", ",", "Locrian", ",", "Melodic", "Minor", ",", "Harmonic", "Minor", ",", "Harmonic", "Major", ",", "Whole", "Tone", ",", "Chromatic", ",", "Octatonic", "Major", ",", "Octatonic", "Minor", ",", "Pentatonic", ",", "Synthetic", "Minor", ",", "Major", "Bitonal", ",", "Minor", "Bitonal", ",", "Polytonal", ",", "Ditone", ",", "Prometheus", ",", "Trans-Pentatonic", ",", "Pelog", ",", "Slendro", ",", "Hungarian-Bartok", ",", "Bulgarian-Bartok", ",", "Asian-Bartok", ",", "Spain", ",", "India-Dharmavati", ",", "Japan", ",", "Peru", ",", "Alaska", ",", "Jewish", "Ahavoh-Rabboh", ",", "Slavic", ",", "Blues" ],
					"maxclass" : "umenu",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 284.83252, 147.5, 165.0, 29.0 ],
					"pattrmode" : 1,
					"presentation" : 1,
					"presentation_rect" : [ 114.0, 242.0, 123.0, 29.0 ],
					"style" : "",
					"varname" : "ubumenu"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-42",
					"maxclass" : "newobj",
					"numinlets" : 2,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "" ],
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 7,
							"minor" : 2,
							"revision" : 4,
							"architecture" : "x86",
							"modernui" : 1
						}
,
						"rect" : [ 405.0, 365.0, 315.0, 284.0 ],
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
						"visible" : 1,
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 9.0,
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 163.0, 160.0, 31.0, 19.0 ],
									"style" : "",
									"text" : "* -1"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 9.0,
									"id" : "obj-2",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 163.0, 180.0, 41.0, 19.0 ],
									"style" : "",
									"text" : "min $1"
								}

							}
, 							{
								"box" : 								{
									"comment" : "(list) from editor",
									"id" : "obj-4",
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 214.0, 21.0, 15.0, 15.0 ],
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 9.0,
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 214.0, 74.0, 56.0, 19.0 ],
									"style" : "",
									"text" : "route text"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 9.0,
									"id" : "obj-6",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 214.0, 138.0, 62.0, 19.0 ],
									"style" : "",
									"text" : "prepend set"
								}

							}
, 							{
								"box" : 								{
									"comment" : "(list) to editor",
									"id" : "obj-7",
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 214.0, 181.0, 15.0, 15.0 ],
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 9.0,
									"id" : "obj-8",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 40.0, 155.0, 94.0, 19.0 ],
									"style" : "",
									"text" : "prepend scale_struct"
								}

							}
, 							{
								"box" : 								{
									"comment" : "(list) to hold please",
									"id" : "obj-9",
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 40.0, 197.0, 15.0, 15.0 ],
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 9.0,
									"id" : "obj-10",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 140.0, 201.0, 43.0, 19.0 ],
									"style" : "",
									"text" : "max $1"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 9.0,
									"id" : "obj-11",
									"maxclass" : "newobj",
									"numinlets" : 2,
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 140.0, 137.0, 33.0, 19.0 ],
									"style" : "",
									"text" : "zl len"
								}

							}
, 							{
								"box" : 								{
									"coll_data" : 									{
										"count" : 35,
										"data" : [ 											{
												"key" : 0,
												"value" : [ 0, 2, 4, 5, 7, 9, 11 ]
											}
, 											{
												"key" : 1,
												"value" : [ 0, 2, 3, 5, 7, 9, 10 ]
											}
, 											{
												"key" : 2,
												"value" : [ 0, 1, 3, 5, 7, 8, 10 ]
											}
, 											{
												"key" : 3,
												"value" : [ 0, 2, 4, 6, 7, 9, 11 ]
											}
, 											{
												"key" : 4,
												"value" : [ 0, 2, 4, 5, 7, 9, 10 ]
											}
, 											{
												"key" : 5,
												"value" : [ 0, 2, 3, 5, 7, 8, 10 ]
											}
, 											{
												"key" : 6,
												"value" : [ 0, 1, 3, 5, 6, 7, 10 ]
											}
, 											{
												"key" : 7,
												"value" : [ 0, 2, 3, 5, 7, 9, 11 ]
											}
, 											{
												"key" : 8,
												"value" : [ 0, 2, 3, 5, 7, 8, 11 ]
											}
, 											{
												"key" : 9,
												"value" : [ 0, 2, 4, 5, 7, 9, 11 ]
											}
, 											{
												"key" : 10,
												"value" : [ 0, 2, 4, 6, 8, 10 ]
											}
, 											{
												"key" : 11,
												"value" : [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ]
											}
, 											{
												"key" : 12,
												"value" : [ 0, 2, 3, 5, 6, 8, 9, 11 ]
											}
, 											{
												"key" : 13,
												"value" : [ 0, 1, 3, 4, 6, 7, 9, 10 ]
											}
, 											{
												"key" : 14,
												"value" : [ 0, 2, 4, 7, 9 ]
											}
, 											{
												"key" : 15,
												"value" : [ 0, 3, 4, 6, 8, 9, 11 ]
											}
, 											{
												"key" : 16,
												"value" : [ 0, 1, 4, 6, 7, 10 ]
											}
, 											{
												"key" : 17,
												"value" : [ 0, 1, 3, 6, 7, 9 ]
											}
, 											{
												"key" : 18,
												"value" : [ 0, 2, 3, 4, 5, 7, 8, 9, 10, 11 ]
											}
, 											{
												"key" : 19,
												"value" : [ 0, 1, 4, 5, 8, 9 ]
											}
, 											{
												"key" : 20,
												"value" : [ 0, 2, 4, 6, 9, 10 ]
											}
, 											{
												"key" : 21,
												"value" : [ 0, 3, 5, 6, 8 ]
											}
, 											{
												"key" : 22,
												"value" : [ 0, 1, 3, 7, 8 ]
											}
, 											{
												"key" : 23,
												"value" : [ 0, 2, 5, 7, 10 ]
											}
, 											{
												"key" : 24,
												"value" : [ 0, 2, 4, 6, 7, 9, 10 ]
											}
, 											{
												"key" : 25,
												"value" : [ 0, 1, 3, 5, 7, 9, 10 ]
											}
, 											{
												"key" : 26,
												"value" : [ 0, 2, 3, 6, 7, 9, 10 ]
											}
, 											{
												"key" : 27,
												"value" : [ 0, 1, 3, 5, 7, 8, 11 ]
											}
, 											{
												"key" : 28,
												"value" : [ 0, 2, 3, 6, 7, 9, 11 ]
											}
, 											{
												"key" : 29,
												"value" : [ 0, 2, 4, 5, 6, 7, 9, 10, 11 ]
											}
, 											{
												"key" : 30,
												"value" : [ 0, 3, 5, 7, 10 ]
											}
, 											{
												"key" : 31,
												"value" : [ 0, 3, 5, 7, 8, 10, 11 ]
											}
, 											{
												"key" : 32,
												"value" : [ 0, 1, 4, 5, 7, 8, 10 ]
											}
, 											{
												"key" : 33,
												"value" : [ 0, 5, 7, 8 ]
											}
, 											{
												"key" : 34,
												"value" : [ 0, 2, 3, 4, 5, 6, 7, 9, 10, 11 ]
											}
 ]
									}
,
									"fontname" : "Arial",
									"fontsize" : 9.0,
									"id" : "obj-12",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 4,
									"outlettype" : [ "", "", "", "" ],
									"patching_rect" : [ 95.0, 74.0, 76.0, 19.0 ],
									"saved_object_attributes" : 									{
										"embed" : 1
									}
,
									"style" : "",
									"text" : "coll scales.coll"
								}

							}
, 							{
								"box" : 								{
									"comment" : "(int) scale number",
									"id" : "obj-13",
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "int" ],
									"patching_rect" : [ 95.0, 25.0, 15.0, 15.0 ],
									"style" : ""
								}

							}
, 							{
								"box" : 								{
									"comment" : "(int) # degrees",
									"id" : "obj-14",
									"maxclass" : "outlet",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 140.0, 221.0, 15.0, 15.0 ],
									"style" : ""
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-1", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-14", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-10", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-10", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-11", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-12", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-12", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-12", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-12", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-13", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-14", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-5", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-4", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-11", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-7", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-6", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-9", 0 ],
									"disabled" : 0,
									"hidden" : 0,
									"source" : [ "obj-8", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 299.0, 247.0, 125.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"style" : "",
						"tags" : ""
					}
,
					"style" : "",
					"text" : "p scaleinfomanager"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-4",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 141.0, 409.0, 98.0, 22.0 ],
					"style" : "",
					"text" : "mxj net.maxhole"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 141.0, 247.0, 83.0, 22.0 ],
					"style" : "",
					"text" : "prepend tonic"
				}

			}
, 			{
				"box" : 				{
					"fontsize" : 18.0,
					"id" : "obj-2",
					"maxclass" : "live.numbox",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "float" ],
					"parameter_enable" : 1,
					"patching_rect" : [ 141.0, 150.0, 56.0, 24.0 ],
					"saved_attribute_attributes" : 					{
						"valueof" : 						{
							"parameter_longname" : "live.numbox",
							"parameter_shortname" : "live.numbox",
							"parameter_type" : 0,
							"parameter_unitstyle" : 8
						}

					}
,
					"varname" : "live.numbox"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-41", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-151", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-42", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-151", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-5", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-151", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-151", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-41", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-42", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4", 0 ],
					"disabled" : 0,
					"hidden" : 0,
					"source" : [ "obj-5", 0 ]
				}

			}
 ],
		"parameters" : 		{
			"obj-2" : [ "live.numbox", "live.numbox", 0 ]
		}
,
		"dependency_cache" : [  ],
		"autosave" : 0
	}

}
