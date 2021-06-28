/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import {
  Switch,
  TextInput,
  View,
  StyleSheet,
  Button,
  Picker,
} from 'react-native';
import * as React from 'react'
import { useState } from 'react';

const styles = StyleSheet.create({
  container: {
    padding: 20,
  },
  regularBorder: {
    height: 50,
    backgroundColor: 'rgba(225,225,225,0.2)',
    borderWidth: 1,
    borderColor: '#ff00ff55',
    marginBottom: 10,
    padding: 10,
    color: '#fff',
  },
  roundBorder: {
    height: 50,
    backgroundColor: 'rgba(0,0,0,0.2)',
    marginBottom: 10,
    padding: 10,
    color: '#000',
    borderRadius: 10.0,
    borderWidth: 10,
    borderColor: '#00ff0055',
  },
  buttonText: {
    color: '#fff',
    textAlign: 'center',
    fontWeight: '700',
    borderRadius: 10.0,
  },
});

export function ControlStyleTestPage() {
  const [showRoundBorder, setShowRoundBorder] = useState(false);

  const onPressShowRoundBorder = () => {
    const previousState = showRoundBorder;
    setShowRoundBorder(!previousState);
  };

  return (
    <View>
      <View testID="control-style-switch-view">
        {
          <Switch
            style={showRoundBorder ? styles.roundBorder : styles.regularBorder}
            thumbColor="blue"
          />
        }
        <TextInput
          style={showRoundBorder ? styles.roundBorder : styles.regularBorder}
          placeholder="TextBox"
          placeholderTextColor="rgba(225,225,225,0.7)"
          editable={false}
        />

        <TextInput
          style={showRoundBorder ? styles.roundBorder : styles.regularBorder}
          placeholder="Password"
          placeholderTextColor="rgba(225,225,225,0.7)"
          secureTextEntry={true}
          editable={false}
        />
        <Picker
          style={showRoundBorder ? styles.roundBorder : styles.regularBorder}
        />
      </View>

      <Button
        title={showRoundBorder ? 'Hide Round Border' : 'Show Round Border'}
        onPress={onPressShowRoundBorder}
        testID="show-border-toggle"
      />
    </View>
  );
}

export const displayName = (_undefined?: string) => {};
export const title = 'LegacyControlStyleTest';
export const description = 'Legacy e2e test for Control Styles';
export const examples = [
  {
    render: function(): JSX.Element {
      return <ControlStyleTestPage />;
    },
  },
];