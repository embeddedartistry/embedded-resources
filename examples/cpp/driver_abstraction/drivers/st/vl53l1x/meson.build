# ST VL53L1X Time-of-Flight Sensor

vl53l1x_files = files(
	'vl53l1x.cpp'
	)

st_vl53l1x_driver = static_library('st_vl53l1x',
	sources: vl53l1x_files,
	include_directories: [
		driver_root_inc,
		driver_core_inc,
		framework_includes
	],
	dependencies: etl_dep
)

st_vl53l1x_driver_dep = declare_dependency(
	link_with: st_vl53l1x_driver,
	include_directories: [
		driver_root_inc,
		driver_core_inc
	],
)
