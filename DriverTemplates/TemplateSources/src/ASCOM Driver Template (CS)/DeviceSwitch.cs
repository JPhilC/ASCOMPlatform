﻿// All lines from line 1 to the device interface implementation region will be discarded by the project wizard when the template is used
// Required code must lie within the device implementation region
// The //ENDOFINSERTEDFILE tag must be the last but one line in this file

using ASCOM;
using ASCOM.Utilities;

class DeviceSwitch
{
    private TraceLogger tl = new TraceLogger();

    #region ISwitchV2 Implementation

    private  short numSwitch;

    /// <summary>
    /// The number of switches managed by this driver
    /// </summary>
    public short MaxSwitch
    {
        get 
        { 
            tl.LogMessage("MaxSwitch Get", numSwitch.ToString());
            return this.numSwitch; 
        }
    }

    /// <summary>
    /// Return the name of switch n
    /// </summary>
    /// <param name="id">The switch number to return</param>
    /// <returns>
    /// The name of the switch
    /// </returns>
    public string GetSwitchName(short id)
    {
        Validate("GetSwitchName", id);
        throw new MethodNotImplementedException("GetSwitchName");
    }

    /// <summary>
    /// Sets a switch name to a specified value
    /// </summary>
    /// <param name="id">The number of the switch whose name is to be set</param>
    /// <param name="name">The name of the switch</param>
    public void SetSwitchName(short id, string name)
    {
        Validate("SetSwitchName", id);
        throw new MethodNotImplementedException("SetSwitchName");
    }

    #region boolean switch members

    /// <summary>
    /// Return the state of switch n
    /// an analogue switch will return true if the value is closer to the maximum than the minimum, otherwise false
    /// </summary>
    /// <param name="id">The switch number to return</param>
    /// <returns>
    /// True or false
    /// </returns>
    public bool GetSwitch(short id)
    {
        Validate("GetSwitch", id);
        throw new MethodNotImplementedException("GetSwitch");
    }

    /// <summary>
    /// Sets a switch to the specified state
    /// If the switch cannot be set then throws a MethodNotImplementedException.
    /// Setting an analogue switch to true will set it to its maximim value and
    /// setting it to false will set it to its minimum value.
    /// </summary>
    /// <param name="id"></param>
    /// <param name="state"></param>
    public void SetSwitch(short id, bool state)
    {
        Validate("SetSwitch", id);
        throw new MethodNotImplementedException("SetSwitch");
    }

    #endregion

    #region analogue members

    /// <summary>
    /// returns the maximum value for this switch
    /// boolean switches must return 1.0
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public double MaxSwitchValue(short id)
    {
        Validate("MaxSwitchValue", id);
        throw new MethodNotImplementedException("MaxSwitchValue");
    }

    /// <summary>
    /// returns the minimum value for this switch
    /// boolean switches must return 0.0
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public double MinSwitchValue(short id)
    {
        Validate("MinSwitchValue", id);
        throw new MethodNotImplementedException("MinSwitchValue");
    }

    /// <summary>
    /// returns the step size that this switch supports. This gives the difference between
    /// successive values of the switch.
    /// The number of values is ((MaxSwitchValue - MinSwitchValue) / SwitchStep) + 1
    /// boolean switches must return 1.0, giving two states.
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public double SwitchStep(short id)
    {
        Validate("SwitchStep", id);
        throw new MethodNotImplementedException("SwitchStep");
    }

    /// <summary>
    /// returns the analogue switch value for switch id
    /// boolean switches will return 1.0 or 0.0
    /// </summary>
    /// <param name="id"></param>
    /// <returns></returns>
    public double GetSwitchValue(short id)
    {
        Validate("GetSwitchValue", id);
        throw new MethodNotImplementedException("GetSwitchValue");
    }

    /// <summary>
    /// set the analogue value for this switch.
    /// If the switch cannot be set then throws a MethodNotImplementedException.
    /// If the value is not between the maximum and minimum then throws an InvalidValueException
    /// boolean switches will be set to true if the value is closer to the maximum than the minimum.
    /// </summary>
    /// <param name="id"></param>
    /// <param name="value"></param>
    public void SetSwitchValue(short id, double value)
    {
        Validate("SetSwitchValue", id, value);
        throw new MethodNotImplementedException("SetSwitchValue");
    }

    #endregion
    #endregion

    #region private methods

    /// <summary>
    /// Checks that the switch id is in range and throws an InvalidValueException if it isn't
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="id">The id.</param>
    private void Validate(string message,short id)
    {
        if (id < 0 || id >= numSwitch)
        {
            tl.LogMessage(message, string.Format("Switch {0} not available, range is 0 to {1}", id, numSwitch -1));
            throw new InvalidValueException(message, id.ToString(), string.Format("0 to {0}", numSwitch - 1));
        }
    }

    /// <summary>
    /// Checks that the switch id and value are in range and throws an
    /// InvalidValueException if they are not.
    /// </summary>
    /// <param name="message">The message.</param>
    /// <param name="id">The id.</param>
    /// <param name="value">The value.</param>
    private void Validate(string message, short id, double value)
    {
        Validate(message, id);
        if (value < MinSwitchValue(id) || value > MaxSwitchValue(id))
        {
 	        throw new InvalidValueException(message, value.ToString(), string.Format("Switch({0}) range {1} to {2}", id, MinSwitchValue(id), MaxSwitchValue(id)));
        }
    }

    #endregion

    //ENDOFINSERTEDFILE
}