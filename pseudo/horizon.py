def getOrderedHorizonHalfEdges(point_node, D):
	# Trouver les edges autour du groupe de faces en conflit avec point_node
	# Critere: Inclure un half-edge si son twin est sur une face non en conflit (invisible)

	horizon = []

	# Trouver un premier half-edge faisant partie de l'horizon
	trouve = False
	for face in point_node.conflits:
		horizon_half_edge = getHalfEdgeTouchingHorizon(face)
		if horizon_half_edge is not None:
			horizon.append(horizon_half_edge)
			break

	# Trouver les half-edge suivants sur l'horizon
	# A partir d'une face, trouver une face adjacente (non-visitee) ayant un half-edge sur l'horizon
	face = horizon[0].face
	faces_visitees = []
	while True:
		faces_visitees.append(face)
		# Boucler sur les half-edges de la face
		premier_half_edge = face.an_edge
		half_edge = premier_half_edge
		continuer = True
		trouve = False
		while continuer:
			adjac_face = half_edge.twin.face
			if adjac_face in faces_visitees:
				continue
			# Verifier si la face adjacente contient un half-edge bordant l'horizon
			horizon_half_edge = getHalfEdgeTouchingHorizon(adjac_face)
			if horizon_half_edge is not None:
				trouve = True
				horizon.append(horizon_half_edge)
				face = adjac_face
				break
			# Suite de la boucle
			half_edge = half_edge.next
			continuer = half_edge != premier_half_edge
		if not trouve:
			# On a termine, car on a visite toutes les faces sur l'horizon
			break

	return horizon

def getHalfEdgeTouchingHorizon(face):
	# Boucler sur les half-edges de la face
	premier_half_edge = face.an_edge
	half_edge = premier_half_edge
	continuer = True 
	while continuer:
		# Si twin fait partie d'une face invisible, on a trouve
		adjac_face = half_edge.twin.face
		if adjac_face not in point_node.conflits:
			return half_edge
		# Suite de la boucle
		half_edge = half_edge.next
		continuer = half_edge != premier_half_edge
	# Si on se rend ici, la face n'est pas a l'interieur de l'horizon
	return None