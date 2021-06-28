/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {
  View,
  StyleSheet,
  Text,
  TextInput,
  TouchableHighlight,
  TouchableOpacity,
  TouchableWithoutFeedback,
  Picker,
} from 'react-native';
import {
  IKeyboardEvent,
  supportKeyboard,
  ViewWindows,
} from 'react-native-windows';

// TextInput2 is used to verify supportKeyboard + focus
const TextInput2 = supportKeyboard(TextInput);

const styles = StyleSheet.create({
  border: {
    borderStyle: 'dotted',
    borderColor: 'black',
  },
  keyComponentRoot: {
    borderWidth: 2,
    flexDirection: 'column',
    marginVertical: 5,
    backgroundColor: 'whitesmoke',
    justifyContent: 'space-around',
  },
  keyEnterVisualizer: {
    margin: 5,
    alignItems: 'center',
    minWidth: 100,
    minHeight: 30,
  },
  textInput: {
    height: 32,
    width: 100,
  },
  blackbox: {height: 30, width: 30, borderColor: 'black', borderWidth: 3},
});

interface IKeyboardFocusComponentState {
  selected: string;
  keyOnKeyDown: string;
  focusMessageHighlight: string;
  focusMessageOpacity: string;
  focusMessageWithoutFeedback: string;
}

const pickerRef = React.createRef<Picker>();
const viewWindowsRef = React.createRef<ViewWindows>();
const textInputRef = React.createRef<TextInput>();
const textInputRef2 = React.createRef<any>();
const touchableHighlightRef = React.createRef<TouchableHighlight>();
const touchableOpacityRef = React.createRef<TouchableOpacity>();
const touchableWithoutFeedbackRef = React.createRef<TouchableWithoutFeedback>();

class KeyboardFocusExample extends React.Component<
  {},
  IKeyboardFocusComponentState
> {
  public constructor(props: {}) {
    super(props);
    this.state = {
      selected: '',
      keyOnKeyDown: 'unknown',
      focusMessageHighlight: '',
      focusMessageOpacity: '',
      focusMessageWithoutFeedback: '',
    };
  }

  public render(): JSX.Element {
    const pickerItems = [
      'View',
      'Picker',
      'TextInput',
      'TextInput2',
      'TouchableHighlight',
      'TouchableOpacity',
    ];

    return (
      <View style={styles.keyComponentRoot}>
        <View>
          <Text>Please select a item to set focus</Text>
        </View>
        <Picker
          selectedValue={this.state.selected}
          onValueChange={this._selectionChanged}>
          {pickerItems.map(item => (
            <Picker.Item key={item} value={item} label={item} />
          ))}
        </Picker>

        <ViewWindows
          style={styles.keyEnterVisualizer}
          focusable={true}
          ref={viewWindowsRef}>
          <Text>View accept focus</Text>
        </ViewWindows>

        <View>
          <Picker ref={pickerRef}>
            <Picker.Item label="Picker accept focus" />
          </Picker>
        </View>
        <View>
          <Text>
            Test Purpose: focus on TextInput, then timeout and blur on
            TextInput2, TextInput still keep focus
          </Text>
          <TextInput
            placeholder="TextInput accept focus"
            style={styles.textInput}
            ref={textInputRef}
          />
        </View>
        <View>
          <Text>
            Test Purpose: focus on TextInput2, then timeout and blur on
            TextInput2, TextInput2 lose focus
          </Text>
          <TextInput2
            onKeyDown={this._textInputKeyDown}
            placeholder="TextInput accept focus"
            style={styles.textInput}
            ref={textInputRef2}
          />
          <Text>Key {this.state.keyOnKeyDown}</Text>
        </View>
        <View>
          <TouchableHighlight
            ref={touchableHighlightRef}
            onFocus={this._touchableHighlightFocus}
            onBlur={this._touchableHighlightBlur}>
            <View style={{width: 50, height: 50, backgroundColor: 'blue'}} />
          </TouchableHighlight>
          <Text>
            Last focus event for TouchableHighlight:{' '}
            {this.state.focusMessageHighlight}
          </Text>
          <TouchableOpacity
            ref={touchableOpacityRef}
            onFocus={this._touchableOpacityFocus}
            onBlur={this._touchableOpacityBlur}>
            <View style={{width: 50, height: 50, backgroundColor: 'yellow'}} />
          </TouchableOpacity>
          <Text>
            Last focus event for TouchableOpacity:{' '}
            {this.state.focusMessageOpacity}
          </Text>
          <TouchableWithoutFeedback
            ref={touchableWithoutFeedbackRef}
            onFocus={this._touchableWithoutFeedbackFocus}
            onBlur={this._touchableWithoutFeedbackBlur}>
            <View style={{width: 50, height: 50, backgroundColor: 'green'}} />
          </TouchableWithoutFeedback>
          <Text>
            Last focus event for TouchableWithoutFeedback:{' '}
            {this.state.focusMessageWithoutFeedback}
          </Text>
        </View>
      </View>
    );
  }

  private readonly _textInputKeyDown = (ev: IKeyboardEvent) => {
    this.setState({keyOnKeyDown: ev.nativeEvent.key});
  };

  private readonly _selectionChanged = (selected: string) => {
    switch (selected) {
      case 'View':
        viewWindowsRef.current && viewWindowsRef.current.focus();
        break;
      case 'Picker':
        // pickerRef.current && pickerRef.current.focus();
        break;
      case 'TextInput':
        textInputRef.current && textInputRef.current.focus();
        if (textInputRef.current) {
          textInputRef.current.focus();
          setTimeout(() => {
            textInputRef2.current && textInputRef2.current.blur();
          }, 10000);
        }
        break;
      case 'TextInput2':
        if (textInputRef2.current) {
          textInputRef2.current.focus();
          setTimeout(() => {
            textInputRef2.current && textInputRef2.current.blur();
          }, 10000);
        }
        break;
      case 'TouchableHighlight':
        touchableHighlightRef.current && touchableHighlightRef.current.focus();
        break;
      case 'TouchableOpacity':
        touchableOpacityRef.current && touchableOpacityRef.current.focus();
        break;
      case 'TouchableWithoutFeedback':
        // TouchableWithoutFeedback doesn't have a focus method, since it doesn't have NativeMethodsMixin applied
        break;
    }
  };

  private readonly _touchableHighlightFocus = () => {
    this.setState({focusMessageHighlight: 'TouchableHighlight onFocus'});
  };
  private readonly _touchableHighlightBlur = () => {
    this.setState({focusMessageHighlight: 'TouchableHighlight onBlur'});
  };
  private readonly _touchableOpacityFocus = () => {
    this.setState({focusMessageOpacity: 'TouchableOpacity onFocus'});
  };
  private readonly _touchableOpacityBlur = () => {
    this.setState({focusMessageOpacity: 'TouchableOpacity onBlur'});
  };
  private readonly _touchableWithoutFeedbackFocus = () => {
    this.setState({
      focusMessageWithoutFeedback: 'TouchableWithoutFeedback onFocus',
    });
  };
  private readonly _touchableWithoutFeedbackBlur = () => {
    this.setState({
      focusMessageWithoutFeedback: 'TouchableWithoutFeedback onBlur',
    });
  };
}
export const displayName = (_undefined?: string) => {};
export const title = 'Keyboard Focus Example';
export const category = 'Basic';
export const description = 'Demo of keyboard focus.';
export const examples = [
  {
    title: 'Keyboard Focus example',
    render(): JSX.Element {
      return <KeyboardFocusExample />;
    },
  },
];
