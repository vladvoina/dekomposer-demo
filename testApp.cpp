#include "testApp.h"

//************************* INSTRUCTIONS ********************************//
/// ONCE THE APPLICATION IS RUNNING
// - press SPACE BAR TO START PLAYBACK
// - click on scrub bar at the top to jump through the track
//
// - to perform the analysis on a different track simply include the
//   track in the bin/data/... folder and change the name of track_filename
//------------------------------------------------------------------------//

void testApp::setup(){

    ofBackground(255*.15);
    ofSetVerticalSync(true);

	/////////////////////////
	/////// MIDI ////////////
	/////////////////////////

	midiOut.listPorts();
	//midiOut.openPort(2);
	channel = 1;
	note_shift = 0;
	
	// ** probability factor for triggering events
	// ** 1 - 100%, 2 - 50%, 3 - 33% and so on
	incrementors = new long[5];
	for (int i=0; i<5; i++) incrementors[i] = 0;

	probabilities = new int[5];
	probabilities[0] = 2;
	probabilities[1] = 3;
	probabilities[2] = 1;
	probabilities[3] = 1;
	probabilities[4] = 2;

	// ** midi notes sent by the event of each track
	notes = new int[5];
	notes[0] = 5;
	notes[1] = 6;
	notes[2] = 7;
	notes[3] = 8;
	notes[4] = 9;
	bool load_from_xml = false; // set to true when you dont want to perform the analysis
	
	/////////////////////////
	/////// ANALYSER ////////
	/////////////////////////

	maxiSettings::setup(44100, 2, 512);
	// ** load sample
	string track_filename = "track24.wav";  // try jimihaze.wav, electronic.wav, 
	sample1.load(ofToDataPath(track_filename));
	//** classification parameters
	const int clusters = 4; // number of clusters to look for and therefore number of separated tracks
	const int dim_reduction = 5; // dimentionality of transformed data

	onsets = new onsetClassification(512, 1024);
	if(!load_from_xml) onsets->analyse(&sample1, clusters, dim_reduction, false, false);

	/////////////////////////
	/////// TIMELINE ////////
	/////////////////////////

    ofxTimeline::removeCocoaMenusFromGlut("AllTracksExample");
	timeline.setup();

	timeline.addAudioTrack("audio", track_filename);
    timeline.setDurationInSeconds(timeline.getAudioTrack("audio")->getDuration());

	//timeline.addFlags("Custom Events");
	//timeline.addFlags("Custom Events 2");
	//timeline.addFlags("Note Shifts");
	
	// ** set a distinct color for each track ** //
	ofColor* bangs_colors;
	bangs_colors = new ofColor[clusters];
	for (int i=0; i<clusters; i++)
	{
	bangs_colors[i].setHsb((i * 255.0/(float)clusters), 255, 255);
	}

	//** set up tracks ** //
	ofxTLColorBangs** bangs = (ofxTLColorBangs**) malloc(clusters*sizeof(ofxTLColorBangs*));
	for(int i=0; i<clusters; i++)
	{
	 timeline.addColorBangs(ofToString(i+1), bangs_colors[i]);
	 bangs[i] = (ofxTLColorBangs*) timeline.getTrack(ofToString(i+1));

	 if(!load_from_xml) bangs[i]->clear(); // clear tracks before annotating
	}

	//**** annotate onsets on each color bang track **** //
	if (!load_from_xml)
	{
		for(int i=0; i<onsets->getOnsetsInMillis()->size(); i++)
		{
		 cout << "-->" << i << endl;
		   for (int j=0; j<clusters;j++)
		   {
			 if(onsets->getIDs()[i] == j)
			 {
			  //ofxTLColorBangs* bangs = (ofxTLColorBangs*) timeline.getTrack(ofToString(j+1));
			  bangs[j]->addKeyframeAtMillis( onsets->getOnsetsInMillis()->at(i));
			  break;
			 }
		   }		
		 }
	}

	timeline.setPageName("Page 1");
	timeline.setCurrentPage(0);

	timeline.enableSnapToOtherKeyframes(false);
	timeline.setLoopType(OF_LOOP_NORMAL);
	
	ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);
}

//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& args){
	
	string track_name = args.track->getName(); 
	/*

    if (track_name == "1")
	{
	 incrementors[0]++; 
	 if (incrementors[0]%probabilities[0] == 0)
	 {
	  cout << "bang fired! " << args.flag << " " << args.track->getName() << endl;
	  midiOut.sendNoteOn(channel, notes[0] + note_shift, 64);
	 } else
	   {
		cout << "bang fired! " << args.flag << " " << args.track->getName() << endl;
	    midiOut.sendNoteOn(channel, 4 + note_shift, 64);
	   }
	} else
	if (track_name == "2")
	{
     incrementors[1]++;
     if (incrementors[1]%probabilities[1] == 0) 
	 {
	  cout << "bang fired! " << args.flag << " " << args.track->getName() << endl;
	  midiOut.sendNoteOn(channel, notes[1] + note_shift, 64);
	 } 
	 else
	   {
		cout << "bang fired! " << args.flag << " " << args.track->getName() << endl;
	    midiOut.sendNoteOn(channel, 80 + note_shift, 64);
	   }

	} else
	if (track_name == "3")
	{
	 incrementors[2]++;
     if (incrementors[2]%probabilities[2] == 0)
     {
	  cout << "bang fired! " << args.flag << " " << args.track->getName() << endl;
	  midiOut.sendNoteOn(channel, notes[2] + note_shift, 64);
	 }

	} else
	if (track_name == "4")
	{
	 incrementors[3]++;
     if (incrementors[3]%probabilities[3] == 0)
	 {
	  cout << "bang fired! " << args.flag << " " << args.track->getName() << endl;
	  midiOut.sendNoteOn(channel, notes[3] + note_shift, 64);
	 }
	} else
	if (track_name == "5")
	{
	 incrementors[4]++;
	 if (incrementors[4]%probabilities[4] == 0) 
	 {
	  cout << "bang fired! " << args.flag << " " << args.track->getName() << endl;
	  midiOut.sendNoteOn(channel, notes[4] + note_shift, 64);
	 } else
	   {
		cout << "bang fired! " << args.flag << " " << args.track->getName() << endl;
	    midiOut.sendNoteOn(channel, 10 + note_shift, 64);
	   }

	} else
	if (track_name == "Custom Events")
	{
     midiOut.sendNoteOn(channel, ofToInt(args.flag), 64);
	} else
	if (track_name == "Note Shifts")
	{
		note_shift = ofToInt(args.flag);
	} else
	if (track_name == "Custom Events 2")
	{
	 midiOut.sendNoteOn(channel, ofToInt(args.flag), 64);
	}
	
	*/

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	timeline.draw();	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::exit()
{
	midiOut.closePort();
}