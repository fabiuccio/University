package mystubs;

import java.util.Observable;

import hcs.interfaces.ClockObserver;
import hcs.model.Clock;
import stubs.IPhysicalBoiler;
import stubs.IPhysicalEnvironment;
import stubs.IPhysicalRoom;

public class CRoom implements IPhysicalRoom , ClockObserver {

	private IPhysicalEnvironment env;
	private IPhysicalBoiler boiler;
	
	private double temperature;
	private double heater;
	private boolean windowOpen;
	private boolean windowTilted;
	private boolean notPresence;
	

	private CRoom() {}
	
	public CRoom(IPhysicalEnvironment env, IPhysicalBoiler boiler) {
		this.env = env;
		this.boiler = boiler;
		temperature = 20;
		heater = 25;
		windowOpen = false;
		windowTilted = false;
		notPresence = false;
		
		Clock.getInstance().addObserver(this);
	}
	
	public boolean closedWindowSwitchOpen() {
		return windowOpen;
	}

	public double getTemperature() {
		return temperature;
	}

	public boolean presenceSwitchOpen() {
		return !notPresence;
	}

	public void setHeaterTarget(double temperature) {
		heater = temperature;
	}

	public void swingWindow() {
		windowOpen = !windowOpen;
		if (windowOpen) {
			windowTilted = false;
		}
	}

	public void tiltWindow() {
		windowTilted = !windowTilted;
		if (windowTilted) {
			windowOpen = false;
		}
	}

	public boolean tiltedWindowSwitchOpen() {
		return windowTilted;
	}

	public void update(Observable o, Object arg) {
		double extTemp = env.getTemperature();
		boolean rain = env.isRainingSwitchClosed();
		
		if (extTemp <= temperature) {
			if (getHeaterCurrent() > temperature) {
				if (windowOpen || windowTilted) {
					// extTemp wins on heater, and temperature cools down
					temperature -= 0.5;
				} /* else {
					// extTemp and heater compensate, so temperature does not change
				} */
			} else { // heater <= temperature
				// extTemp and heater are cooler that the room, the room cools too
				if (windowOpen || windowTilted) {
					// temperature cools down quickly
					if (rain) { // if it rains even quicker
						temperature -= 1.5;
					} else {
						temperature -= 1;
					}
				} else { // slowly
					temperature -= 0.5;
				}
			}
		} else { // extTemp > temperature
			if (getHeaterCurrent() > temperature) {
				temperature += 1;
			} else { // heater <= temperature
				if ((windowOpen || windowTilted) && !rain) {
					// extTemp wins on heater, temperature warms up
					temperature += 0.5;
				} else {
					// ext temp is actively heating, while heater is passively not heating (but NEVER lowing the
					// temperature down), so the temperature is slowly growing up (slowly because the window is closed)
					temperature += 0.1;
				}
			}
		}
	}

	public double getHeaterTarget() {
		return heater;
	}
	
	public double getHeaterCurrent() {
		return Math.min(boiler.getCurrentTemperature(), heater);
	}

}
