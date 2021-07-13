/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

// This is a port of TextExample.android.js
// Image inline in Text removed

import React from 'react';
import {
  /*Image,*/ StyleSheet,
  Text,
  View,
  TextStyle,
  TouchableWithoutFeedback,
} from 'react-native';
const RNTesterBlock = require('../../components/RNTesterBlock');
const RNTesterPage = require('../../components/RNTesterPage');

export class Entity extends React.Component<{}> {
  public render() {
    return (
      <Text style={{fontWeight: 'bold', color: '#527fe4'}}>
        {this.props.children}
      </Text>
    );
  }
}

interface IAttributeTogglerState {
  fontWeight: 'bold' | 'normal';
  fontSize: number;
}
export class AttributeToggler extends React.Component<
  {},
  IAttributeTogglerState
> {
  public state: IAttributeTogglerState = {fontWeight: 'bold', fontSize: 15};

  public toggleWeight = () => {
    this.setState({
      fontWeight: this.state.fontWeight === 'bold' ? 'normal' : 'bold',
    });
  };

  public increaseSize = () => {
    this.setState({
      fontSize: this.state.fontSize + 1,
    });
  };

  public render() {
    const curStyle: TextStyle = {
      fontWeight: this.state.fontWeight,
      fontSize: this.state.fontSize,
    };
    return (
      <View>
        <Text style={curStyle}>
          Tap the controls below to change attributes.
        </Text>
        <Text>
          <Text>
            See how it will even work on{' '}
            <Text style={curStyle}>this nested text</Text>
          </Text>
        </Text>
        <Text>
          <Text onPress={this.toggleWeight}>Toggle Weight</Text>
          {' (with highlight onPress)'}
        </Text>
        <Text onPress={this.increaseSize} suppressHighlighting={true}>
          Increase Size (suppressHighlighting true)
        </Text>
      </View>
    );
  }
}

export class BackgroundColorDemo extends React.Component<{}> {
  public render() {
    return (
      <View testID={'text-background-color'}>
        <Text style={{color: 'pink'}}>Outer pink</Text>
        <Text style={{}}>
          Outer <Text style={{color: 'red'}}>red nested</Text>
        </Text>
        <Text style={{}}>
          Outer{' '}
          <Text style={{backgroundColor: 'blue', color: 'white'}}>
            nested white on blue
          </Text>
        </Text>
        <Text style={{color: 'pink'}}>
          Outer pink <Text style={{color: 'red'}}>nested red</Text>
        </Text>
        <Text style={{backgroundColor: 'green'}}>
          Outer on green{' '}
          <Text style={{color: 'white'}}>nested white on inherit green</Text>
        </Text>
        <Text style={{backgroundColor: 'green', color: 'orange'}}>
          Outer orange on green{' '}
          <Text style={{backgroundColor: 'blue', color: 'white'}}>
            nested white on blue
          </Text>
        </Text>
        <Text style={{color: 'orange'}}>
          Outer orange{' '}
          <Text style={{backgroundColor: 'blue', color: 'white'}}>
            nested white on blue
          </Text>
        </Text>
        <Text style={{color: 'orange'}}>
          <Text style={{backgroundColor: 'blue'}}>
            nested orange inherit on blue
          </Text>
        </Text>

        <Text>
          Outer no_color{' '}
          <Text style={{backgroundColor: 'green', color: 'white'}}>
            START_NESTED green{' '}
            <Text style={{backgroundColor: 'blue', color: 'magenta'}}>
              DEEPER_NESTED magenta on blue
            </Text>{' '}
            END_NESTED
          </Text>{' '}
          attributes.
        </Text>
        <Text>
          Outer no_color{' '}
          <Text>
            START_NESTED no_color{' '}
            <Text style={{backgroundColor: 'blue'}}>DEEPER_NESTED blue</Text>{' '}
            END_NESTED
          </Text>{' '}
          attributes.
        </Text>
        <Text>
          Outer no_color{' '}
          <Text style={{backgroundColor: 'green'}}>
            START_NESTED green <Text>DEEPER_NESTED inherit green</Text>{' '}
            END_NESTED
          </Text>{' '}
          attributes.
        </Text>
        <Text style={{backgroundColor: 'red'}}>
          Outer red{' '}
          <Text>
            START_NESTED inherit red <Text>DEEPER_NESTED inherit red</Text>{' '}
            END_NESTED
          </Text>{' '}
          attributes.
        </Text>
        <Text style={{backgroundColor: 'red'}}>
          Outer red{' '}
          <Text style={{backgroundColor: 'green'}}>
            START_NESTED green{' '}
            <Text style={{backgroundColor: 'blue'}}>DEEPER_NESTED blue</Text>{' '}
            END_NESTED
          </Text>{' '}
          attributes.
        </Text>
      </View>
    );
  }
}

export class TextExample extends React.Component<
  {},
  {toggle1: boolean; toggle2: boolean; toggle3: boolean}
> {
  constructor(props: any) {
    super(props);
    this.state = {toggle1: false, toggle2: false, toggle3: false};
  }

  public render() {
    const lorumIpsum =
      'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed dapibus felis eget augue condimentum suscipit. Suspendisse hendrerit, libero aliquet malesuada tempor, urna nibh consectetur tellus, vitae efficitur quam erat non mi. Maecenas vitae eros sit amet quam vestibulum porta sed sit amet tellus. Fusce quis lectus congue, fringilla arcu id, luctus urna. Cras sagittis ornare mauris sit amet dictum. Vestibulum feugiat laoreet fringilla. Vivamus ac diam vehicula felis venenatis sagittis vitae ultrices elit. Curabitur libero augue, laoreet quis orci vitae, congue euismod massa. Aenean nec odio sed urna vehicula fermentum non a magna. Quisque ut commodo neque, eget eleifend odio. Sed sit amet lacinia sem. Suspendisse in metus in purus scelerisque vestibulum. Nam metus dui, efficitur nec metus non, tincidunt pharetra sapien. Praesent id convallis metus, ut malesuada arcu. Quisque quam libero, pharetra eu tellus ac, aliquam fringilla erat. Quisque tempus in lorem ac suscipit.';

    return (
      <RNTesterPage>
        <RNTesterBlock title="textTransform">
          <View testID={'text-transform'}>
            <Text style={{textTransform: 'uppercase'}}>
              <Text>This</Text> text should be uppercased.
            </Text>
            <Text style={{textTransform: 'lowercase'}}>
              This TEXT SHOULD be lowercased.
            </Text>
            <Text style={{textTransform: 'capitalize'}}>
              This text should be CAPITALIZED.
            </Text>
            <Text style={{textTransform: 'capitalize'}}>
              Mixed:{' '}
              <Text style={{textTransform: 'uppercase'}}>uppercase </Text>
              <Text style={{textTransform: 'lowercase'}}>LoWeRcAsE </Text>
              <Text style={{textTransform: 'capitalize'}}>
                capitalize each word
              </Text>
            </Text>
            <Text>
              Should be "ABC":
              <Text style={{textTransform: 'uppercase'}}>
                a
                <Text>
                  b<Text>c</Text>
                </Text>
              </Text>
            </Text>
            <Text>
              Should be "XyZ":
              <Text style={{textTransform: 'uppercase'}}>
                x<Text style={{textTransform: 'none'}}>y</Text>z
              </Text>
            </Text>
            <Text>
              <Text>
                Should be "xYz":
                <Text>
                  x<Text style={{textTransform: 'uppercase'}}>y</Text>z
                </Text>
              </Text>
            </Text>
            <Text onPress={() => this.setState({toggle1: !this.state.toggle1})}>
              Click to toggle uppercase:{' '}
              <Text
                style={{
                  textTransform: this.state.toggle1 ? 'uppercase' : 'none',
                }}>
                Hello
              </Text>
            </Text>
            <Text onPress={() => this.setState({toggle2: !this.state.toggle2})}>
              <Text>
                Click to change raw text:{' '}
                <Text style={{textTransform: 'uppercase'}}>
                  Hello {this.state.toggle2 ? 'Earth' : 'World'}
                </Text>
              </Text>
            </Text>
            <TouchableWithoutFeedback
              onPress={() => this.setState({toggle3: !this.state.toggle3})}>
              <View>
                <Text>
                  Click to toggle fast text on next line (should remain
                  uppercase):
                </Text>
                <Text style={{textTransform: 'uppercase'}}>
                  {this.state.toggle3 ? 'Hello' : 'Howdy'}
                </Text>
              </View>
            </TouchableWithoutFeedback>
          </View>
        </RNTesterBlock>
        <RNTesterBlock title="Wrap">
          <Text testID={'text-wrap'}>
            The text should wrap if it goes on multiple lines. See, this is
            going to the next line. {lorumIpsum}
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Padding">
          <Text style={{padding: 10}} testID={'text-padding'}>
            This text is indented by 10px padding on all sides.
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Font Family">
          <Text style={{fontFamily: 'sans-serif'}}>Sans-Serif</Text>
          <Text
            style={{fontFamily: 'sans-serif', fontWeight: 'bold'}}
            testID={'text-font-family'}>
            Sans-Serif Bold
          </Text>
          <Text style={{fontFamily: 'serif'}}>Serif</Text>
          <Text style={{fontFamily: 'serif', fontWeight: 'bold'}}>
            Serif Bold
          </Text>
          <Text style={{fontFamily: 'monospace'}}>Monospace</Text>
          <Text style={{fontFamily: 'monospace', fontWeight: 'bold'}}>
            Monospace Bold (After 5.0)
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Android Material Design fonts">
          <View style={{flexDirection: 'row', alignItems: 'flex-start'}}>
            <View style={{flex: 1}}>
              <Text style={{fontFamily: 'sans-serif'}}>Roboto Regular</Text>
              <Text style={{fontFamily: 'sans-serif', fontStyle: 'italic'}}>
                Roboto Italic
              </Text>
              <Text style={{fontFamily: 'sans-serif', fontWeight: 'bold'}}>
                Roboto Bold
              </Text>
              <Text
                style={{
                  fontFamily: 'sans-serif',
                  fontStyle: 'italic',
                  fontWeight: 'bold',
                }}>
                Roboto Bold Italic
              </Text>
              <Text style={{fontFamily: 'sans-serif-light'}}>Roboto Light</Text>
              <Text
                style={{fontFamily: 'sans-serif-light', fontStyle: 'italic'}}>
                Roboto Light Italic
              </Text>
              <Text style={{fontFamily: 'sans-serif-thin'}}>
                Roboto Thin (After 4.2)
              </Text>
              <Text
                style={{fontFamily: 'sans-serif-thin', fontStyle: 'italic'}}>
                Roboto Thin Italic (After 4.2)
              </Text>
              <Text style={{fontFamily: 'sans-serif-condensed'}}>
                Roboto Condensed
              </Text>
              <Text
                style={{
                  fontFamily: 'sans-serif-condensed',
                  fontStyle: 'italic',
                }}>
                Roboto Condensed Italic
              </Text>
              <Text
                style={{
                  fontFamily: 'sans-serif-condensed',
                  fontWeight: 'bold',
                }}>
                Roboto Condensed Bold
              </Text>
              <Text
                style={{
                  fontFamily: 'sans-serif-condensed',
                  fontStyle: 'italic',
                  fontWeight: 'bold',
                }}>
                Roboto Condensed Bold Italic
              </Text>
              <Text style={{fontFamily: 'sans-serif-medium'}}>
                Roboto Medium (After 5.0)
              </Text>
              <Text
                style={{fontFamily: 'sans-serif-medium', fontStyle: 'italic'}}>
                Roboto Medium Italic (After 5.0)
              </Text>
            </View>
          </View>
        </RNTesterBlock>
        <RNTesterBlock title="Custom Fonts">
          <View style={{flexDirection: 'row', alignItems: 'flex-start'}}>
            <View style={{flex: 1}}>
              <Text style={{fontFamily: 'notoserif'}}>NotoSerif Regular</Text>
              <Text
                style={{
                  fontFamily: 'notoserif',
                  fontStyle: 'italic',
                  fontWeight: 'bold',
                }}>
                NotoSerif Bold Italic
              </Text>
              <Text style={{fontFamily: 'notoserif', fontStyle: 'italic'}}>
                NotoSerif Italic (Missing Font file)
              </Text>
            </View>
          </View>
        </RNTesterBlock>

        <RNTesterBlock title="Font Size">
          <Text style={{fontSize: 23}} testID={'text-size'}>
            Size 23
          </Text>
          <Text style={{fontSize: 8}}>Size 8</Text>
        </RNTesterBlock>
        <RNTesterBlock title="Color">
          <Text style={{color: 'red'}} testID={'text-color'}>
            Red color
          </Text>
          <Text style={{color: 'blue'}}>Blue color</Text>
        </RNTesterBlock>
        <RNTesterBlock title="Font Weight">
          <Text style={{fontWeight: 'bold'}}>Move fast and be bold</Text>
          <Text style={{fontWeight: 'normal'}}>Move fast and be bold</Text>
        </RNTesterBlock>
        <RNTesterBlock title="Font Style">
          <Text style={{fontStyle: 'italic'}}>Move fast and be bold</Text>
          <Text style={{fontStyle: 'normal'}}>Move fast and be bold</Text>
        </RNTesterBlock>
        <RNTesterBlock title="Font Style and Weight">
          <Text style={{fontStyle: 'italic', fontWeight: 'bold'}}>
            Move fast and be bold
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Text Decoration">
          <Text
            style={{textDecorationLine: 'underline'}}
            testID={'text-decoration-underline'}>
            Solid underline
          </Text>
          <Text style={{textDecorationLine: 'none'}}>None textDecoration</Text>
          <Text
            style={{
              textDecorationLine: 'line-through',
              textDecorationStyle: 'solid',
            }}
            testID={'text-decoration-solid-linethru'}>
            Solid line-through
          </Text>
          <Text style={{textDecorationLine: 'underline line-through'}}>
            Both underline and line-through
          </Text>
          <Text>
            Mixed text with{' '}
            <Text style={{textDecorationLine: 'underline'}}>underline</Text> and{' '}
            <Text style={{textDecorationLine: 'line-through'}}>
              line-through
            </Text>{' '}
            text nodes
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Nested">
          <Text onPress={() => console.log('1st')} testID={'text-outer-color'}>
            (Normal text,
            <Text style={{color: 'red', fontWeight: 'bold'}}>
              (R)red
              <Text style={{color: 'green', fontWeight: 'normal'}}>
                (G)green
                <Text style={{color: 'blue', fontWeight: 'bold'}}>
                  (B)blue
                  <Text style={{color: 'cyan', fontWeight: 'normal'}}>
                    (C)cyan
                    <Text style={{color: 'magenta', fontWeight: 'bold'}}>
                      (M)magenta
                      <Text style={{color: 'yellow', fontWeight: 'normal'}}>
                        (Y)yellow
                        <Text style={{color: 'black', fontWeight: 'bold'}}>
                          (K)black
                        </Text>
                      </Text>
                    </Text>
                  </Text>
                </Text>
              </Text>
            </Text>
            <Text
              style={{fontWeight: 'bold'}}
              onPress={() => console.log('2nd')}>
              (and bold
              <Text
                style={{fontStyle: 'italic', fontSize: 11, color: '#527fe4'}}
                onPress={() => console.log('3rd')}>
                (and tiny bold italic blue
                <Text
                  style={{fontWeight: 'normal', fontStyle: 'normal'}}
                  onPress={() => console.log('4th')}>
                  (and tiny normal blue)
                </Text>
                )
              </Text>
              )
            </Text>
            )
          </Text>
          <Text
            style={{fontFamily: 'serif'}}
            onPress={() => console.log('1st')}>
            (Serif
            <Text
              style={{fontStyle: 'italic', fontWeight: 'bold'}}
              onPress={() => console.log('2nd')}>
              (Serif Bold Italic
              <Text
                style={{
                  fontFamily: 'monospace',
                  fontStyle: 'normal',
                  fontWeight: 'normal',
                }}
                onPress={() => console.log('3rd')}>
                (Monospace Normal
                <Text
                  style={{fontFamily: 'sans-serif', fontWeight: 'bold'}}
                  onPress={() => console.log('4th')}>
                  (Sans-Serif Bold
                  <Text
                    style={{fontWeight: 'normal'}}
                    onPress={() => console.log('5th')}>
                    (and Sans-Serif Normal)
                  </Text>
                  )
                </Text>
                )
              </Text>
              )
            </Text>
            )
          </Text>
          <Text style={{fontSize: 12}}>
            <Entity>Entity Name</Entity>
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Text Align">
          <View testID={'text-align'}>
            <Text>auto (default) - english LTR</Text>
            <Text>أحب اللغة العربية auto (default) - arabic RTL</Text>
            <Text style={{textAlign: 'left'}}>
              left left left left left left left left left left left left left
              left left
            </Text>
            <Text style={{textAlign: 'center'}}>
              center center center center center center center center center
              center center
            </Text>
            <Text style={{textAlign: 'right'}}>
              right right right right right right right right right right right
              right right
            </Text>
          </View>
        </RNTesterBlock>
        <RNTesterBlock title="Unicode">
          <View testID={'text-unicode'}>
            <View style={{flexDirection: 'row'}}>
              <Text style={{backgroundColor: 'red'}}>
                星际争霸是世界上最好的游戏。
              </Text>
            </View>
            <View>
              <Text style={{backgroundColor: 'red'}}>
                星际争霸是世界上最好的游戏。
              </Text>
            </View>
            <View style={{alignItems: 'center'}}>
              <Text style={{backgroundColor: 'red'}}>
                星际争霸是世界上最好的游戏。
              </Text>
            </View>
            <View>
              <Text style={{backgroundColor: 'red'}}>
                星际争霸是世界上最好的游戏。星际争霸是世界上最好的游戏。星际争霸是世界上最好的游戏。星际争霸是世界上最好的游戏。
              </Text>
            </View>
          </View>
        </RNTesterBlock>
        <RNTesterBlock title="Spaces">
          <Text>
            A {'generated'} {'string'} and some &nbsp;&nbsp;&nbsp; spaces
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Line Height">
          <View testID={'text-line-height'}>
            <Text style={{lineHeight: 35}}>
              Holisticly formulate inexpensive ideas before best-of-breed
              benefits. <Text style={{fontSize: 20}}>Continually</Text> expedite
              magnetic potentialities rather than client-focused interfaces.
            </Text>
            <Text style={{lineHeight: 15}}>
              Holisticly formulate inexpensive ideas before best-of-breed
              benefits. <Text style={{fontSize: 20}}>Continually</Text> expedite
              magnetic potentialities rather than client-focused interfaces.
            </Text>
          </View>
        </RNTesterBlock>
        <RNTesterBlock title="Letter Spacing">
          <View testID={'text-letter-spacing'}>
            <Text style={{letterSpacing: 0}}>letterSpacing = 0</Text>
            <Text style={{letterSpacing: 2, marginTop: 5}}>
              letterSpacing = 2
            </Text>
            <Text style={{letterSpacing: 9, marginTop: 5}}>
              letterSpacing = 9
            </Text>
            <View style={{flexDirection: 'row'}}>
              <Text
                style={{
                  fontSize: 12,
                  letterSpacing: 2,
                  backgroundColor: 'fuchsia',
                  marginTop: 5,
                }}>
                With size and background color
              </Text>
            </View>
            <Text style={{letterSpacing: -1, marginTop: 5}}>
              letterSpacing = -1
            </Text>
            <Text
              style={{
                letterSpacing: 3,
                backgroundColor: '#dddddd',
                marginTop: 5,
              }}>
              [letterSpacing = 3]
              <Text style={{letterSpacing: 0, backgroundColor: '#bbbbbb'}}>
                [Nested letterSpacing = 0]
              </Text>
              <Text style={{letterSpacing: 6, backgroundColor: '#eeeeee'}}>
                [Nested letterSpacing = 6]
              </Text>
            </Text>
          </View>
        </RNTesterBlock>
        <RNTesterBlock title="Empty Text">
          <Text />
        </RNTesterBlock>
        <RNTesterBlock title="Toggling Attributes">
          <AttributeToggler />
        </RNTesterBlock>

        <RNTesterBlock title="backgroundColor new examples">
          <BackgroundColorDemo />
        </RNTesterBlock>

        <RNTesterBlock title="backgroundColor attribute">
          <Text style={{backgroundColor: '#ffaaaa'}}>
            Red background,
            <Text style={{backgroundColor: '#aaaaff'}}>
              {' '}
              blue background,
              <Text>
                {' '}
                inherited blue background,
                <Text style={{backgroundColor: '#aaffaa'}}>
                  {' '}
                  nested green background.
                </Text>
              </Text>
            </Text>
          </Text>
          <Text style={{backgroundColor: 'rgba(100, 100, 100, 0.3)'}}>
            Same alpha as background,
            <Text>
              Inherited alpha from background,
              <Text style={{backgroundColor: 'rgba(100, 100, 100, 0.3)'}}>
                Reapply alpha
              </Text>
            </Text>
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="containerBackgroundColor attribute">
          <View style={{flexDirection: 'row', height: 85}}>
            <View style={{backgroundColor: '#ffaaaa', width: 150}} />
            <View style={{backgroundColor: '#aaaaff', width: 150}} />
          </View>
          <Text style={[styles.backgroundColorText, {top: -80}]}>
            Default containerBackgroundColor (inherited) + backgroundColor wash
          </Text>
          <Text
            style={[
              styles.backgroundColorText,
              {top: -70, backgroundColor: 'transparent'},
            ]}>
            {"containerBackgroundColor: 'transparent' + backgroundColor wash"}
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="numberOfLines attribute">
          <Text style={{marginTop: 0, fontStyle: 'italic'}}>1</Text>
          <Text numberOfLines={1} testID={'text-one-line'}>
            Maximum of one line no matter now much I write here. If I keep
            writing it{"'"}ll just truncate after one line. {lorumIpsum}
          </Text>

          <Text style={{marginTop: 20, fontStyle: 'italic'}}>2</Text>
          <Text numberOfLines={2}>
            Maximum of two lines no matter now much I write here. If I keep
            writing it{"'"}ll just truncate after two lines. {lorumIpsum}
          </Text>

          <Text style={{marginTop: 20, fontStyle: 'italic'}}>
            (default) infinity
          </Text>
          <Text>
            No maximum lines specified no matter now much I write here. If I
            keep writing it{"'"}ll just keep going and going. {lorumIpsum}
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="selectable attribute">
          <Text selectable testID={'text-selectable'}>
            This text is selectable if you click-and-hold, and will offer the
            native Android selection menus.
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="selectionColor attribute">
          <Text
            selectable
            selectionColor="orange"
            testID={'text-selection-color'}>
            This text will have a orange highlight on selection.
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="selectable onPress">
          <Text
            selectable
            testID={'text-selection-onPress'}>
            Pressing{' '}
            <Text
              style={{color: 'blue', textDecorationLine: 'underline'}}
              onPress={() => alert('Pressed!')}>
                here
              </Text>
              {' '}text will show an alert.
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Inline images">
          <Text>
            This text contains an inline image{' '}
            {/*
            <Image source={require('./flux.png')} />.*/} Neat,
            huh?
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Text shadow">
          <Text
            style={{
              fontSize: 20,
              textShadowOffset: {width: 2, height: 2},
              textShadowRadius: 1,
              textShadowColor: '#00cccc',
            }}
            testID={'text-shadow'}>
            Demo text shadow
          </Text>
        </RNTesterBlock>
        <RNTesterBlock title="Ellipsize mode">
          <View testID={'text-ellipsize'}>
            <Text style={{marginTop: 0, fontStyle: 'italic'}}>
              (default) tail
            </Text>
            <Text numberOfLines={1}>
              This very long text should be truncated with dots in the end.{' '}
              {lorumIpsum}
            </Text>

            <Text style={{marginTop: 20, fontStyle: 'italic'}}>middle</Text>
            <Text ellipsizeMode="middle" numberOfLines={1}>
              This very long text should be truncated with dots in the middle.{' '}
              {lorumIpsum}
            </Text>

            <Text style={{marginTop: 20, fontStyle: 'italic'}}>head</Text>
            <Text ellipsizeMode="head" numberOfLines={1}>
              This very long text should be truncated with dots in the
              beginning. {lorumIpsum}
            </Text>

            <Text style={{marginTop: 20, fontStyle: 'italic'}}>clip</Text>
            <Text ellipsizeMode="clip" numberOfLines={1}>
              This very long text should be clipped and this will not be
              visible. {lorumIpsum}
            </Text>
          </View>
        </RNTesterBlock>
        <RNTesterBlock title="Include Font Padding">
          <View
            style={{
              flexDirection: 'row',
              justifyContent: 'space-around',
              marginBottom: 10,
            }}
            testID={'text-font-padding'}>
            <View style={{alignItems: 'center'}}>
              <Text style={styles.includeFontPaddingText}>Ey</Text>
              <Text>Default</Text>
            </View>
            <View style={{alignItems: 'center'}}>
              <Text
                style={[
                  styles.includeFontPaddingText,
                  {includeFontPadding: false, marginLeft: 10},
                ]}>
                Ey
              </Text>
              <Text>includeFontPadding: false</Text>
            </View>
          </View>
          <Text>
            By default Android will put extra space above text to allow for
            upper-case accents or other ascenders. With some fonts, this can
            make text look slightly misaligned when centered vertically.
          </Text>
        </RNTesterBlock>

        <RNTesterBlock title="Text With Border">
          <View testID={'text-border'}>
            <Text style={styles.borderedTextSimple}>
              Sample bordered text with default styling.
            </Text>

            <Text style={styles.borderedText}>
              Some more bordered text + a tad of CSS.{'\n'}
              <Text style={{borderColor: 'red', borderWidth: 5}}>
                1st nested - border specifcied but ignored.{'\n'}
                <Text style={{borderColor: 'yellow', borderWidth: 4}}>
                  2nd Inside text!
                </Text>
              </Text>
            </Text>

            <Text>
              This text is{' '}
              <Text
                style={{color: 'red', borderWidth: 1, borderColor: 'black'}}>
                outlined{' '}
              </Text>
              and laid out within the normal text run, so will wrap etc as
              normal text.
            </Text>
          </View>
        </RNTesterBlock>
      </RNTesterPage>
    );
  }
}

export const styles = StyleSheet.create({
  backgroundColorText: {
    left: 5,
    backgroundColor: 'rgba(100, 100, 100, 0.3)',
  },
  includeFontPaddingText: {
    fontSize: 120,
    fontFamily: 'sans-serif',
    backgroundColor: '#EEEEEE',
    color: '#000000',
    textAlignVertical: 'center',
    alignSelf: 'center',
  },
  borderedText: {
    margin: 100,
    fontSize: 18,
    fontWeight: 'bold',
    textAlign: 'center',
    borderWidth: 3,
    borderColor: 'green',
    padding: 30,
  },
  borderedTextSimple: {
    fontSize: 18,
    borderWidth: 2,
    borderColor: 'black',
    width: 400,
  },
});

export const displayName = (_undefined?: string) => {};
export const title = 'Text';
export const description = 'Base component for rendering styled text.';
export const examples = [
  {
    render: function(): JSX.Element {
      return <TextExample />;
    },
  },
];
