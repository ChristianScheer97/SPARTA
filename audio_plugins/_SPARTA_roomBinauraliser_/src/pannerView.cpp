/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.9

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "pannerView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
const float icon_size = 8.0f;


//[/MiscUserDefs]

//==============================================================================
pannerView::pannerView (PluginProcessor* ownerFilter, int _width, int _height)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (492, 286);


    //[Constructor] You can add your own custom stuff here..
    hVst = ownerFilter;
    hBin = hVst->getFXHandle();
    width = _width;
    height = _height;

    for(int src=0; src<MAX_NUM_INPUTS; src++){
        SourceIcons[src].setBounds(width - width*(roombinauraliser_getSourceAzi_deg(hBin, src) + 180.0f)/360.f - icon_size/2.0f,
                                   height - height*(roombinauraliser_getSourceElev_deg(hBin, src) + 90.0f)/180.0f - icon_size/2.0f,
                                   icon_size,
                                   icon_size);
    }
    NSources = roombinauraliser_getNumSources(hBin);
    NLoudspeakers = roombinauraliser_getNDirs(hBin)>MAX_NUM_OUT_DIRS? MAX_NUM_OUT_DIRS : roombinauraliser_getNDirs(hBin);
    for(int ls=0; ls<NLoudspeakers; ls++){
        LoudspeakerIcons[ls].setBounds(width - width*(roombinauraliser_getHRIRAzi_deg(hBin, ls) + 180.0f)/360.f - icon_size/2.0f,
                                       height - height*(roombinauraliser_getHRIRElev_deg(hBin, ls)+90.0f)/180.0f - icon_size/2.0f,
                                       icon_size,
                                       icon_size);
    }
    showInputs = true;
    showOutputs = true;
    sourceIconIsClicked = false;
    soloActive = false;
    //[/Constructor]
}

pannerView::~pannerView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void pannerView::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = 492, height = 286;
        juce::Colour fillColour1 = juce::Colour (0xff4e4e4e), fillColour2 = juce::Colour (0xff202020);
        juce::Colour strokeColour = juce::Colour (0xff9e9e9e);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setGradientFill (juce::ColourGradient (fillColour1,
                                                 248.0f - 0.0f + x,
                                                 0.0f - 0.0f + y,
                                                 fillColour2,
                                                 248.0f - 0.0f + x,
                                                 248.0f - 0.0f + y,
                                                 false));
        g.fillRect (x, y, width, height);
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 1);

    }

    //[UserPaint] Add your own custom painting code here..


    /* Draw Grid lines and labels */
    int numGridLinesX = 8;
    int numGridLinesY = numGridLinesX / 2;
    g.setColour(Colours::white);
    g.setOpacity(0.75f);

    g.drawLine(0.0f, height / 2.0f, width, height / 2.0f, 1.0f);
    g.drawLine(width / 2.0f, 0, width / 2.0f, height, 1.0f);

    for (int i = 0; i <= numGridLinesX; i++) {
        g.setOpacity(0.1f);
        g.drawLine((float)i*width / (float)numGridLinesX, 0, (float)i*width / (float)numGridLinesX, height, 1.0f);
        g.setOpacity(0.75f);
        if (i <= numGridLinesX / 2) {
            g.drawText(String((int)(360 / 2 - i * (int)360 / numGridLinesX)) + "\xc2\xb0",
                       (float)i*width / (float)numGridLinesX, height / 2, 40, 20, Justification::centred, true);
        }
        else {
            g.drawText(String((int)(360 / 2 - i * (int)360 / numGridLinesX)) + "\xc2\xb0",
                       (float)i*width / (float)numGridLinesX - 40, height / 2, 40, 20, Justification::centred, true);
        }
    }

    for (int i = 0; i <= numGridLinesY; i++) {
        g.setOpacity(0.1f);
        g.drawLine(0, (float)i*height / (float)numGridLinesY, width, (float)i*height / (float)numGridLinesY, 1.0f);
        g.setOpacity(0.75f);
        if (i <= numGridLinesY / 2) {
            g.drawText(String((int)(180 / 2 - i * (int)180 / numGridLinesY)) + "\xc2\xb0",
                       width / 2.0f, (float)i*height / (float)numGridLinesY, 40, 20, Justification::centred, true);
        }
        else {
            g.drawText(String((int)(180 / 2 - i * (int)180 / numGridLinesY)) + "\xc2\xb0",
                       width / 2.0f, (float)i*height / (float)numGridLinesY - 20, 40, 20, Justification::centred, true);
        }
    }

    if(showOutputs){
        /* Draw loudspeaker/HRIR icons */
        for(int ls=0; ls<NLoudspeakers; ls++){
            /* icon */
            g.setColour(Colour::fromFloatRGBA(0.5f, 1.0f, 0.1f, 1.0f));
            g.setOpacity(0.3f);
            g.fillRect(LoudspeakerIcons[ls]);
        }
    }

    if(showInputs){
        /* Draw Source icons */
        for(int src=0; src<NSources; src++){
            if (!hiddenEmitters.contains(src)) { // non-hidden emitters
                /* icon */
                //g.setColour(Colour::fromFloatRGBA(1.0-((float)src/(float)NSources), 0.3f, ((float)src/(float)NSources), 1.0f));
                g.setColour(Colour::fromFloatRGBA(1.0f, 0.13f, 0.0f, 1.0f));
                //setColourGradient(g, (float)src/(float)NSources);
                g.setOpacity(0.2f);
                g.fillEllipse(SourceIcons[src].expanded(10.0f,10.0f));
                g.setOpacity(0.4f);
                g.fillEllipse(SourceIcons[src].expanded(5.0f, 5.0f));
                g.setOpacity(0.85f);
                g.fillEllipse(SourceIcons[src]);
                /* icon ID */
                g.setColour(Colours::white);
                g.setOpacity(0.9f);
                g.drawText(String(src+1), SourceIcons[src].expanded(10.0f, 0.0f), Justification::centred, true); // .translated(icon_size, -icon_size)
            }
            else // hidden emitters
            {
                g.setColour(Colour::fromFloatRGBA(0.5f, 0.5f, 0.5f, 0.15f));
                g.setOpacity(0.35f);
                g.fillEllipse(SourceIcons[src].expanded(7.0f,7.0f));
                /* icon ID */
                g.setColour(Colours::lightgrey);
                g.setOpacity(1.0f);
                g.drawText(String(src+1), SourceIcons[src].expanded(10.0f, 0.0f), Justification::centred, true); // .translated(icon_size, -icon_size)
            }

        }
    }

    /* Draw SOLO ACTIVE */
    if(soloActive){
        g.setColour(Colours::red.brighter(0.2f));
        g.drawSingleLineText("SoloActive", 5, 15);
    }

    //[/UserPaint]
}

void pannerView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void pannerView::mouseDown (const juce::MouseEvent& e)
{
    //[UserCode_mouseDown] -- Add your code here...
    for(int i=0; i<NSources; i++){
        Rectangle<int> icon_int;
        icon_int.setBounds(SourceIcons[i].getX(),
                           SourceIcons[i].getY(),
                           SourceIcons[i].getWidth(),
                           SourceIcons[i].getHeight());

        if(icon_int.expanded(6, 6).contains(e.getMouseDownPosition())){
            sourceIconIsClicked = true;
            indexOfClickedSource = i;

            // Solo on ALT
            if(e.mods.isAltDown()){
                if (!soloActive)
                    roombinauraliser_setSourceSolo(hBin, i);
                else
                    roombinauraliser_setUnSolo(hBin);
                soloActive = !soloActive;
            }
            break;
        }
    }
    //[/UserCode_mouseDown]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void pannerView::refreshPanView()
{
    for(int src=0; src<MAX_NUM_INPUTS; src++){
        SourceIcons[src].setBounds(width - width*(roombinauraliser_getSourceAzi_deg(hBin, src) + 180.0f)/360.f - icon_size/2.0f,
                                   height - height*(roombinauraliser_getSourceElev_deg(hBin, src) + 90.0f)/180.0f - icon_size/2.0f,
                                   icon_size,
                                   icon_size);
    }
    NSources = roombinauraliser_getNumSources(hBin);
    NLoudspeakers = roombinauraliser_getNDirs(hBin)>MAX_NUM_OUT_DIRS ? MAX_NUM_OUT_DIRS : roombinauraliser_getNDirs(hBin);
    for(int ls=0; ls<NLoudspeakers; ls++){
        LoudspeakerIcons[ls].setBounds(width - width*(roombinauraliser_getHRIRAzi_deg(hBin, ls) + 180.0f)/360.f - icon_size/2.0f,
                                       height - height*(roombinauraliser_getHRIRElev_deg(hBin, ls) + 90.0f)/180.0f - icon_size/2.0f,
                                       icon_size,
                                       icon_size);
    }

    repaint();
}

void pannerView::hideEmitter(int idx, bool isActive)
{
    if (isActive) hiddenEmitters.removeAllInstancesOf(idx);
    else hiddenEmitters.add(idx);
    for (int i=0; i<roombinauraliser_getNumSources(hBin); i++)
    {
        if (hiddenEmitters.contains(i))
            roombinauraliser_muteSource(hBin, i, true);
        else
            roombinauraliser_muteSource(hBin, i, false);
    }
    repaint();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="pannerView" componentName=""
                 parentClasses="public Component" constructorParams="PluginProcessor* ownerFilter, int _width, int _height"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="492" initialHeight="286">
  <METHODS>
    <METHOD name="mouseDown (const MouseEvent&amp; e)"/>
  </METHODS>
  <BACKGROUND backgroundColour="323e44">
    <RECT pos="0 0 492 286" fill="linear: 248 0, 248 248, 0=ff4e4e4e, 1=ff202020"
          hasStroke="1" stroke="1, mitered, butt" strokeColour="solid: ff9e9e9e"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
